#include "wavparser.h"
#include <QFile>
#include <QDataStream>
#include <QDebug>

WavData WavParser::parse(const QString &filePath)
{
    WavData data;
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Cannot open file:" << filePath;
        return data;
    }

    QDataStream stream(&file);
    stream.setByteOrder(QDataStream::LittleEndian);

    char header[12];
    stream.readRawData(header, 12);
    if (memcmp(header, "RIFF", 4) != 0 || memcmp(header + 8, "WAVE", 4) != 0) {
        qWarning() << "Invalid WAV file";
        return data;
    }

    while (!stream.atEnd()) {
        char chunkId[4];
        quint32 chunkSize;
        stream.readRawData(chunkId, 4);
        stream >> chunkSize;

        if (memcmp(chunkId, "fmt ", 4) == 0) {
            quint16 audioFormat;
            stream >> audioFormat;
            stream >> data.channels;
            stream >> data.sampleRate;
            stream.skipRawData(6);
            stream >> data.bitsPerSample;
            if (chunkSize > 16) stream.skipRawData(chunkSize - 16);
        } else if (memcmp(chunkId, "data", 4) == 0) {
            int totalSamples = chunkSize / (data.bitsPerSample / 8);
            data.samples.resize(totalSamples);
            for (int i = 0; i < totalSamples; ++i) {
                if (data.bitsPerSample == 16) {
                    qint16 sample;
                    stream >> sample;
                    data.samples[i] = sample;
                } else {
                    break;
                }
            }
        } else {
            stream.skipRawData(chunkSize);
        }
    }

    return data;
}

QVector<WavData> WavParser::chunkAudio(const WavData &data, int chunkSizeMs)
{
    QVector<WavData> chunks;
    if (data.sampleRate <= 0) return chunks;

    int samplesPerChunk = data.sampleRate * (chunkSizeMs / 1000.0);
    int totalChunks = (data.samples.size() + samplesPerChunk - 1) / samplesPerChunk;

    for (int i = 0; i < totalChunks; ++i) {
        WavData chunk;
        chunk.sampleRate = data.sampleRate;
        chunk.bitsPerSample = data.bitsPerSample;
        chunk.channels = data.channels;

        int start = i * samplesPerChunk;
        int end = qMin(start + samplesPerChunk, data.samples.size());
        chunk.samples = data.samples.mid(start, end - start);

        chunks.append(chunk);
    }

    return chunks;
}
