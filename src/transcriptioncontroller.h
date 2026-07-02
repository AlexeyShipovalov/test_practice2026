#pragma once
#include <QObject>
#include <QThread>
#include "sttworker.h"

class TranscriptionController : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool isModelReady READ isModelReady NOTIFY modelReadyChanged)
    Q_PROPERTY(bool isTranscribing READ isTranscribing NOTIFY transcribingChanged)
    Q_PROPERTY(int progress READ progress NOTIFY progressChanged)
public:
    explicit TranscriptionController(QObject *parent = nullptr);
    ~TranscriptionController();

    bool isModelReady() const { return m_modelReady; }
    bool isTranscribing() const { return m_transcribing; }
    int progress() const { return m_progress; }

    Q_INVOKABLE void startTranscription(const QString &audioPath, const QString &title);
    Q_INVOKABLE void cancelTranscription();

signals:
    void modelReadyChanged();
    void transcribingChanged();
    void progressChanged();
    void transcriptionFinished(const QString &title, const QString &text, const QString &audioPath);
    void errorOccurred(const QString &error);

    // Добавьте эти две строки:
    void startModelLoad(const QString &modelPath);
    void startProcessing(const QString &audioPath);

private slots:
    void handleModelLoaded(bool success, qint64 loadTimeMs, const QString &error);
    void handleFinished(const QString &text, qint64 processTimeMs, int totalDurationSec);

private:
    QThread m_thread;
    SttWorker *m_worker;
    bool m_modelReady = false;
    bool m_transcribing = false;
    int m_progress = 0;
    QString m_currentTitle;
    QString m_currentAudioPath;
};
