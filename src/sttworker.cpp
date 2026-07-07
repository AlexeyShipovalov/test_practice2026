#include "sttworker.h"
#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonArray>
#include <QTextStream>
#include <cmath>
#include <kiss_fft.h>

SttWorker::SttWorker(QObject *parent) : QObject(parent)
{
    m_env = std::make_unique<Ort::Env>(ORT_LOGGING_LEVEL_WARNING, "SttWorker");
}

void SttWorker::loadModel(const QString &modelPath)
{
    QElapsedTimer timer;
    timer.start();

    try {
        Ort::SessionOptions options;
        options.SetIntraOpNumThreads(2);
        options.SetGraphOptimizationLevel(GraphOptimizationLevel::ORT_ENABLE_ALL);

        m_session = std::make_unique<Ort::Session>(*m_env, modelPath.toStdString().c_str(), options);
        m_modelReady = true;

        // Берем директорию из пути к модели
        QString dataDir = QFileInfo(modelPath).absolutePath() + "/";

        // 1. Загрузка словаря
        QFile vocabFile(dataDir + "vocab.json");
        if (vocabFile.open(QIODevice::ReadOnly)) {
            QJsonArray arr = QJsonDocument::fromJson(vocabFile.readAll()).array();
            m_vocab.clear();
            for (const auto& v : arr) m_vocab.append(v.toString());
            qDebug() << "Словарь загружен, размер:" << m_vocab.size();
        } else {
            qDebug() << "Внимание: vocab.json не найден!";
        }

        // 2. Загрузка Mel-фильтров (КРИТИЧЕСКИ ВАЖНО!)
        m_melFilters.clear();
        QFile melFile(dataDir + "mel_filters.txt");
        if (melFile.open(QIODevice::ReadOnly)) {
            QTextStream in(&melFile);
            while (!in.atEnd()) {
                QString line = in.readLine();
                QStringList vals = line.split(' ', QString::SkipEmptyParts);
                std::vector<float> row;
                for (const QString& v : vals) {
                    row.push_back(v.toFloat());
                }
                if (!row.empty()) {
                    m_melFilters.push_back(row);
                }
            }
            qDebug() << "Mel-фильтры загружены, размер:" << m_melFilters.size();
        } else {
            qDebug() << "Внимание: mel_filters.txt не найден!";
        }

        emit modelLoaded(true, timer.elapsed(), "");
    } catch (const Ort::Exception &e) {
        emit modelLoaded(false, timer.elapsed(), QString(e.what()));
    }
}

// Чтение WAV (только 16-bit PCM)
std::vector<float> SttWorker::readWavFile(const QString &filePath, int &sampleRate) {
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) return {};

    WavHeader header;
    file.read(reinterpret_cast<char*>(&header), sizeof(WavHeader));

    sampleRate = header.sampleRate;
    if (header.numChannels != 1 || header.bitsPerSample != 16) {
        return {}; // Поддерживаем только моно 16-бит
    }

    qint64 dataSize = file.size() - sizeof(WavHeader);
    int numSamples = dataSize / (header.bitsPerSample / 8);
    std::vector<int16_t> pcmData(numSamples);
    file.read(reinterpret_cast<char*>(pcmData.data()), dataSize);

    std::vector<float> audio(numSamples);
    for (int i = 0; i < numSamples; ++i) {
        audio[i] = static_cast<float>(pcmData[i]) / 32768.0f;
    }
    return audio;
}

// Вычисление Mel-спектрограммы
std::vector<float> SttWorker::computeMelSpectrogram(const std::vector<float>& audio) {
    const int fftSize = 512;
    const int hopSize = 160;
    const int nMels = 64; // GigaAM использует 64!

    int padSize = fftSize / 2;
    std::vector<float> paddedAudio(padSize + audio.size() + padSize, 0.0f);
    std::copy(audio.begin(), audio.end(), paddedAudio.begin() + padSize);

    int numFrames = (paddedAudio.size() - fftSize) / hopSize + 1;
    if (numFrames <= 0) return {};

    kiss_fft_cfg cfg = kiss_fft_alloc(fftSize, 0, nullptr, nullptr);
    std::vector<kiss_fft_cpx> in(fftSize), out(fftSize);

    std::vector<float> melData(nMels * numFrames, 0.0f);

    for (int i = 0; i < numFrames; ++i) {
        for (int j = 0; j < fftSize; ++j) {
            float window = 0.5f * (1.0f - cosf(2.0f * M_PI * j / (fftSize - 1)));
            in[j].r = paddedAudio[i * hopSize + j] * window;
            in[j].i = 0.0f;
        }
        kiss_fft(cfg, in.data(), out.data());

        std::vector<float> powerSpec(fftSize / 2 + 1);
        for (int j = 0; j < fftSize / 2 + 1; ++j) {
            powerSpec[j] = out[j].r * out[j].r + out[j].i * out[j].i;
        }

        for (int m = 0; m < nMels; ++m) {
            float melVal = 0.0f;
            if (m < static_cast<int>(m_melFilters.size())) {
                for (int j = 0; j < fftSize / 2 + 1; ++j) {
                    if (j < static_cast<int>(m_melFilters[m].size())) {
                        melVal += m_melFilters[m][j] * powerSpec[j];
                    }
                }
            }
            melData[m * numFrames + i] = logf(melVal + 1e-6f);
        }
    }
    kiss_fft_free(cfg);

    // Для ONNX [1, nMels, time] нам не нужен transpose, melData уже имеет нужную структуру
    return melData;
}

// Декодирование CTC (Greedy)
QString SttWorker::decodeCtc(const Ort::Value& logits) {
    auto tensorInfo = logits.GetTensorTypeAndShapeInfo();
    auto shape = tensorInfo.GetShape();
    int seqLen = shape[1];
    int vocabSize = shape[2];

    const float* logitData = logits.GetTensorData<float>();

    QString result;
    int prevToken = -1;

    for (int t = 0; t < seqLen; ++t) {
        int maxIdx = 0;
        float maxVal = -std::numeric_limits<float>::max();
        for (int v = 0; v < vocabSize; ++v) {
            float val = logitData[t * vocabSize + v];
            if (val > maxVal) {
                maxVal = val;
                maxIdx = v;
            }
        }

        if (maxIdx != 0 && maxIdx != prevToken) {
            if (maxIdx < m_vocab.size()) {
                result += m_vocab[maxIdx];
            }
        }
        prevToken = maxIdx;
    }
    return result;
}

void SttWorker::processAudio(const QString &audioPath)
{
    m_cancel = false;
    QElapsedTimer timer;
    timer.start();

    int sampleRate;
    std::vector<float> audio = readWavFile(audioPath, sampleRate);
    if (audio.empty()) {
        emit errorOccurred("Не удалось прочитать WAV файл");
        return;
    }

    const int chunkSizeSec = 30;
    const int chunkSizeSamples = sampleRate * chunkSizeSec;
    int totalChunks = (audio.size() + chunkSizeSamples - 1) / chunkSizeSamples;
    QString fullText = "";

    for (int chunkIdx = 0; chunkIdx < totalChunks; ++chunkIdx) {
        if (m_cancel) {
            emit errorOccurred("Расшифровка отменена");
            return;
        }

        int startSample = chunkIdx * chunkSizeSamples;
        int endSample = std::min(startSample + chunkSizeSamples, (int)audio.size());
        std::vector<float> chunk(audio.begin() + startSample, audio.begin() + endSample);

        std::vector<float> melData = computeMelSpectrogram(chunk);
        if (melData.empty()) continue;

        // ИСПРАВЛЕНО: делим на 64 (nMels), а не на 80!
        int timeFrames = melData.size() / 64;

        // ИСПРАВЛЕНО: Форма тензора 64, а не 80!
        std::vector<int64_t> inputShape = {1, 64, timeFrames};
        std::vector<int64_t> lengthShape = {1};
        std::vector<int64_t> lengthData = {timeFrames};

        Ort::MemoryInfo memInfo = Ort::MemoryInfo::CreateCpu(OrtArenaAllocator, OrtMemTypeDefault);

        Ort::Value inputMel = Ort::Value::CreateTensor<float>(
            memInfo, melData.data(), melData.size(), inputShape.data(), inputShape.size());

        Ort::Value inputLength = Ort::Value::CreateTensor<int64_t>(
            memInfo, lengthData.data(), lengthData.size(), lengthShape.data(), lengthShape.size());

        const char* inputNames[] = {"mel_features", "feature_lengths"};
        const char* outputNames[] = {"logits"};

        try {
            // ИСПРАВЛЕНО: Правильная передача массива тензоров в ONNX Runtime
            std::vector<Ort::Value> inputTensors;
            inputTensors.push_back(std::move(inputMel));
            inputTensors.push_back(std::move(inputLength));

            auto outputs = m_session->Run(
                Ort::RunOptions{nullptr},
                inputNames, inputTensors.data(), 2,
                outputNames, 1
                );

            QString chunkText = decodeCtc(outputs[0]);
            fullText += chunkText + " ";
            emit chunkProcessed(chunkText);

        } catch (const Ort::Exception &e) {
            emit errorOccurred(QString("ONNX Error: ") + e.what());
            return;
        }

        emit progressChanged(static_cast<int>((chunkIdx + 1) * 100.0 / totalChunks));
    }

    qint64 elapsedMs = timer.elapsed();
    int durationSec = audio.size() / sampleRate;
    emit finished(fullText.trimmed(), elapsedMs, durationSec);
}
