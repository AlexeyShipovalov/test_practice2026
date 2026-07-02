#include "transcriptioncontroller.h"
#include <QStandardPaths>
#include <QDir>
#include <QDebug>

TranscriptionController::TranscriptionController(QObject *parent) : QObject(parent)
{
    m_worker = new SttWorker;
    m_worker->moveToThread(&m_thread);

    connect(&m_thread, &QThread::finished, m_worker, &QObject::deleteLater);

    // Запросы туда
    connect(this, &TranscriptionController::startModelLoad, m_worker, &SttWorker::loadModel);
    connect(this, &TranscriptionController::startProcessing, m_worker, &SttWorker::processAudio);

    // Результаты обратно
    connect(m_worker, &SttWorker::modelLoaded, this, &TranscriptionController::handleModelLoaded);
    connect(m_worker, &SttWorker::progressChanged, this, [this](int p){
        m_progress = p; emit progressChanged();
    });
    connect(m_worker, &SttWorker::chunkProcessed, this, [](QString){ /* можно обновлять UI в реальном времени */ });
    connect(m_worker, &SttWorker::finished, this, &TranscriptionController::handleFinished);
    connect(m_worker, &SttWorker::errorOccurred, this, &TranscriptionController::errorOccurred);

    m_thread.start();

    // Запуск загрузки модели в фоне
    // QString modelDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    // QDir().mkpath(modelDir + "/models");
    // emit startModelLoad(modelDir + "/models/gigaam_v3.onnx");
}

TranscriptionController::~TranscriptionController()
{
    m_worker->setCancelFlag(true);
    m_thread.quit();
    m_thread.wait(); // Корректное завершение
}

void TranscriptionController::startTranscription(const QString &audioPath, const QString &title)
{
    if (m_transcribing) return;
    m_currentTitle = title;
    m_currentAudioPath = audioPath;
    m_transcribing = true;
    m_progress = 0;
    emit transcribingChanged();
    emit progressChanged();
    emit startProcessing(audioPath);
}

void TranscriptionController::cancelTranscription()
{
    m_worker->setCancelFlag(true);
}

void TranscriptionController::handleModelLoaded(bool success, qint64 loadTimeMs, const QString &error)
{
    m_modelReady = success;
    if (!success) emit errorOccurred("Ошибка загрузки модели: " + error);
    qDebug() << "Model load time:" << loadTimeMs << "ms";
    emit modelReadyChanged();
}

void TranscriptionController::handleFinished(const QString &text, qint64 processTimeMs, int totalDurationSec)
{
    m_transcribing = false;
    m_progress = 100;
    emit transcribingChanged();
    emit progressChanged();

    double ratio = (double)processTimeMs / 1000.0 / totalDurationSec;
    qDebug() << "Processing time per 1 min of audio:" << (ratio * 60) << "seconds";

    emit transcriptionFinished(m_currentTitle, text, m_currentAudioPath);
}
