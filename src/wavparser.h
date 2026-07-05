#pragma once
#include <QString>
#include <QVector>

struct WavData {
    int sampleRate = 16000;
    short bitsPerSample = 16;
    short channels = 1;
    QVector<short> samples;
};

class WavParser {
public:
    static WavData parse(const QString &filePath);
    static QVector<WavData> chunkAudio(const WavData &data, int chunkSizeMs = 10000);
};
