#include "sttworker.h"
#include <QFile>
#include <QFileInfo>
#include <QDebug>
#include <QThread>
#include <cstdlib>
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
        emit modelLoaded(true, timer.elapsed(), "");
    } catch (const Ort::Exception& e) {
        emit modelLoaded(false, timer.elapsed(), QString(e.what()));
    }
}

void SttWorker::processAudio(const QString &audioPath)
{
    //if (!m_modelReady) {
    //    emit errorOccurred("Модель не загружена");
    //    return;
    //}

    QFile file(audioPath);
    if (!file.exists() || file.size() < 1000) {
        emit errorOccurred("Файл пуст или слишком короткий");
        return;
    }

    QElapsedTimer timer;
    timer.start();

    // Заглушка: Здесь происходит реальное чтение WAV, нарезка на чанки (например по 10 сек),
    // извлечение Mel-спектрограмм через kissfft и вызов m_session->Run().
    // Для демонстрации логики:
    int totalChunks = 10;
    QString resultText = "";

    for (int i = 0; i < totalChunks; ++i) {
        if (m_cancel) {
            emit errorOccurred("Расшифровка отменена");
            return;
        }

        // Имитация инференса
        QThread::msleep(500);

        QString chunkText = QString("Чанк %1 расшифрован. ").arg(i+1);
        resultText += chunkText;
        emit chunkProcessed(chunkText);
        emit progressChanged(static_cast<int>((i + 1) * 100.0 / totalChunks));
    }

    qint64 elapsedMs = timer.elapsed();
    // Замер: время на 1 минуту аудио (допустим аудио 60 сек)
    emit finished(resultText, elapsedMs, 60);
}
