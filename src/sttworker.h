#pragma once
#include <QElapsedTimer>
#include <QObject>
#include <atomic>
#include <memory>
#include <vector>
#include <onnxruntime_cxx_api.h>

// Структура для заголовка WAV
struct WavHeader {
    char riff[4];
    uint32_t fileSize;
    char wave[4];
    char fmt[4];
    uint32_t fmtSize;
    uint16_t audioFormat;
    uint16_t numChannels;
    uint32_t sampleRate;
    uint32_t byteRate;
    uint16_t blockAlign;
    uint16_t bitsPerSample;
};

class SttWorker : public QObject
{
    Q_OBJECT
public:
    explicit SttWorker(QObject *parent = nullptr);
    void setCancelFlag(bool cancel) { m_cancel = cancel; }

public slots:
    void loadModel(const QString &modelPath);
    void processAudio(const QString &audioPath);

signals:
    void modelLoaded(bool success, qint64 loadTimeMs, const QString &error);
    void chunkProcessed(QString text);
    void progressChanged(int percent);
    void finished(QString fullText, qint64 processTimeMs, int totalDurationSec);
    void errorOccurred(QString error);

private:
    std::vector<float> readWavFile(const QString &filePath, int &sampleRate);
    std::vector<float> computeMelSpectrogram(const std::vector<float>& audio);
    QString decodeCtc(const Ort::Value& logits);

    std::unique_ptr<Ort::Env> m_env;
    std::unique_ptr<Ort::Session> m_session;
    std::atomic<bool> m_cancel{false};
    bool m_modelReady = false;

    // Mel фильтры и словарь
    std::vector<std::vector<float>> m_melFilters;
    QStringList m_vocab;
};
