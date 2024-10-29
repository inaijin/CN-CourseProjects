#ifndef AUDIOINPUT_H
#define AUDIOINPUT_H

#include <QAudioFormat>
#include <QAudioSource>
#include <QIODevice>
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

signals:
    void encodedDataReady(const QByteArray &data);

protected:
    qint64 readData(char *data, qint64 maxlen) override;
    qint64 writeData(const char *data, qint64 len) override;

private slots:
    void processAudioBuffer();

private:
    void initializeOpusEncoder();

    QAudioFormat audioFormat;
    QAudioSource *audioSource;
    QIODevice *inputDevice;

    OpusEncoder *opusEncoder;
    int opusFrameSize;
    QByteArray audioBuffer;
};

#endif // AUDIOINPUT_H