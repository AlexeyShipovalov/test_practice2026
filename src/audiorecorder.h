#pragma once
#include <QObject>
#include <QAudioRecorder>
#include <QTimer>

class AudioRecorder : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool recording READ recording NOTIFY recordingChanged)
    Q_PROPERTY(QString duration READ duration NOTIFY durationChanged)
    Q_PROPERTY(int level READ level NOTIFY levelChanged)
public:
    explicit AudioRecorder(QObject *parent = nullptr);

    bool recording() const { return m_recording; }
    QString duration() const { return m_duration; }
    int level() const { return m_level; }

    Q_INVOKABLE void startRecording();
    Q_INVOKABLE QString stopRecording();

signals:
    void recordingChanged();
    void durationChanged();
    void levelChanged();

private slots:
    void updateDuration(qint64 duration);

private:
    QAudioRecorder *m_recorder;
    QTimer *m_levelTimer;
    bool m_recording = false;
    QString m_duration = "00:00";
    int m_level = 0;
    QString m_currentFile;
};
