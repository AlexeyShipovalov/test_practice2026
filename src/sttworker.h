#pragma once
#include <QObject>
#include <onnxruntime_cxx_api.h>
#include <QElapsedTimer>
#include <atomic>
#include <memory>
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
    std::unique_ptr<Ort::Env> m_env;
    std::unique_ptr<Ort::Session> m_session;
    std::unique_ptr<Ort::SessionOptions> m_sessionOptions;
    std::atomic<bool> m_cancel{false};
    bool m_modelReady = false;
};
