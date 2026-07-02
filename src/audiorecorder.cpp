#include "audiorecorder.h"
#include <QStandardPaths>
#include <QDateTime>
#include <QDir>
#include <QUrl>
#include <QTimer>
#include <cstdlib> // <--- Используем стандартный rand()

AudioRecorder::AudioRecorder(QObject *parent) : QObject(parent),
    m_recorder(new QAudioRecorder(this))
{
    connect(m_recorder, &QAudioRecorder::durationChanged, this, &AudioRecorder::updateDuration);

    m_levelTimer = new QTimer(this);
    m_levelTimer->setInterval(100);
    connect(m_levelTimer, &QTimer::timeout, this, [this]() {
        if (m_recording) {
            m_level = 30 + (rand() % 60); // Имитация сигнала от 30 до 90
            emit levelChanged();
        }
    });
}

void AudioRecorder::startRecording()
{
    QString dataDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir().mkpath(dataDir);
    m_currentFile = dataDir + "/" + QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss") + ".wav";

    QAudioEncoderSettings settings;
    settings.setCodec("audio/pcm");
    settings.setSampleRate(16000);
    settings.setBitRate(16);
    settings.setChannelCount(1);
    settings.setEncodingMode(QMultimedia::ConstantBitRateEncoding);

    m_recorder->setEncodingSettings(settings);
    m_recorder->setOutputLocation(QUrl::fromLocalFile(m_currentFile));
    m_recorder->record();

    m_recording = true;
    m_levelTimer->start();
    emit recordingChanged();
}

QString AudioRecorder::stopRecording()
{
    m_recorder->stop();
    m_levelTimer->stop();
    m_recording = false;
    m_duration = "00:00";
    m_level = 0;
    emit recordingChanged();
    emit durationChanged();
    emit levelChanged();
    return m_currentFile;
}
void AudioRecorder::updateDuration(qint64 duration)
{
    // Если реальная запись работает, она перезапишет мок
    int sec = duration / 1000;
    m_duration = QString("%1:%2").arg(sec / 60, 2, 10, QChar('0')).arg(sec % 60, 2, 10, QChar('0'));
    emit durationChanged();
}
