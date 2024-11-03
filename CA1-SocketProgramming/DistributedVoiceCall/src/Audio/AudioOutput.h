#ifndef AUDIOOUTPUT_H
#define AUDIOOUTPUT_H

#include <QAudioFormat>
#include <QAudioSink>
#include <QObject>
#include <QMutex>
#include <QQueue>

#include <opus.h>

class AudioOutput : public QObject
{
    Q_OBJECT

public:
    explicit AudioOutput(QObject *parent = nullptr);
    ~AudioOutput();

    void addData(const QByteArray &data);

private slots:
    void play();

private:
    QAudioFormat audioFormat;
    QAudioSink *audioSink;
    QIODevice *outputDevice;
    QMutex mutex;
    QQueue<QByteArray> dataQueue;
    OpusDecoder *opusDecoder;
    int opusFrameSize;
};

#endif // AUDIOOUTPUT_H
