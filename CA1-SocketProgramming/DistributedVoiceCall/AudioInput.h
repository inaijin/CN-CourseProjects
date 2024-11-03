#ifndef AUDIOINPUT_H
#define AUDIOINPUT_H

#include <QIODevice>
#include <QAudioSource>
#include <QAudioFormat>
#include <QByteArray>
#include <QObject>
#include <opus.h>

class AudioInput : public QIODevice
{
    Q_OBJECT

public:
    explicit AudioInput(QObject *parent = nullptr);
    ~AudioInput();

    void start();
    void stop();

    qint64 readData(char *data, qint64 maxlen) override;
    qint64 writeData(const char *data, qint64 len) override;

signals:
    void encodedDataReady(const QByteArray &data);

private:
    void initializeOpusEncoder();
    void processAudioBuffer();

    QAudioSource *audioSource;
    QIODevice *inputDevice;
    QAudioFormat audioFormat;

    OpusEncoder *opusEncoder;
    int opusFrameSize;
    QByteArray audioBuffer;
};

#endif // AUDIOINPUT_H
