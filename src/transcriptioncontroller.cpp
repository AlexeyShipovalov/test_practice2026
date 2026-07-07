#include "transcriptioncontroller.h"
#include <QDebug>

#include "transcriptioncontroller.h"
#include <QDebug>
#include <QFile>

TranscriptionController::TranscriptionController(QObject *parent)
    : QObject(parent)
{
    m_worker = new SttWorker;
    m_worker->moveToThread(&m_thread);

    connect(&m_thread, &QThread::finished, m_worker, &QObject::deleteLater);

    connect(this, &TranscriptionController::startModelLoad, m_worker, &SttWorker::loadModel);
    connect(this, &TranscriptionController::startProcessing, m_worker, &SttWorker::processAudio);

    connect(m_worker, &SttWorker::modelLoaded, this, &TranscriptionController::handleModelLoaded);
    connect(m_worker, &SttWorker::progressChanged, this, [this](int p) {
        m_progress = p;
        emit progressChanged();
    });

    connect(m_worker, &SttWorker::errorOccurred, this, [this](const QString &err) {
        m_transcribing = false;
        m_progress = 0;
        emit transcribingChanged();
        emit progressChanged();
        emit errorOccurred(err);
    });

    connect(m_worker, &SttWorker::finished, this, &TranscriptionController::handleFinished);

    m_thread.start();

    // ИСПРАВЛЕНО: Проверяем наличие файла и грузим ОДИН раз
    QString modelPath = QStringLiteral("/usr/share/ru.repeater.repeater/models/gigaam_encoder_decoder.onnx");
    if (QFile::exists(modelPath)) {
        emit startModelLoad(modelPath);
    } else {
        emit errorOccurred("Файл модели не найден: " + modelPath);
    }
}


TranscriptionController::~TranscriptionController()
{
    m_worker->setCancelFlag(true);
    m_thread.quit();
    m_thread.wait();
}

void TranscriptionController::startTranscription(const QString &audioPath, const QString &title)
{
    if (m_transcribing)
        return;
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
    // Если мы отменили, мы должны дать знать UI, что процесс завершен
    // Но флаг m_cancel в воркере сработает позже, поэтому UI обновит лямбда выше (errorOccurred)
    m_worker->setCancelFlag(true);
}

void TranscriptionController::handleModelLoaded(bool success, qint64 loadTimeMs, const QString &error)
{
    m_modelReady = success;
    if (!success)
        emit errorOccurred("Ошибка загрузки модели: " + error);
    qDebug() << "Model load time:" << loadTimeMs << "ms";
    emit modelReadyChanged();
}

void TranscriptionController::handleFinished(const QString &text, qint64 processTimeMs, int totalDurationSec)
{
    m_transcribing = false;
    m_progress = 100;
    emit transcribingChanged();
    emit progressChanged();

    double ratio = (double) processTimeMs / 1000.0 / totalDurationSec;
    qDebug() << "Processing time per 1 min of audio:" << (ratio * 60) << "seconds";

    emit transcriptionFinished(m_currentTitle, text, m_currentAudioPath);
}
