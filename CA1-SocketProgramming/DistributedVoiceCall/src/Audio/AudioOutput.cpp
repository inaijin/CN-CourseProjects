#include "AudioOutput.h"
#include <QDebug>

AudioOutput::AudioOutput(QObject *parent)
    : QObject(parent),
    audioSink(nullptr),
    outputDevice(nullptr),
    opusDecoder(nullptr),
    opusFrameSize(0)
{
    audioFormat.setSampleRate(48000);
    audioFormat.setChannelCount(1);
    audioFormat.setSampleFormat(QAudioFormat::Int16);

    int error;
    opusDecoder = opus_decoder_create(
        audioFormat.sampleRate(),
        audioFormat.channelCount(),
        &error
        );

    if (error != OPUS_OK) {
        qCritical() << "Failed to create Opus decoder:" << opus_strerror(error);
        opusDecoder = nullptr;
        return;
    }

    int frameDurationMs = 20;
    opusFrameSize = (audioFormat.sampleRate() / 1000) * frameDurationMs;

    audioSink = new QAudioSink(audioFormat, this);

    outputDevice = audioSink->start();
    if (!outputDevice) {
        qCritical() << "Failed to start audio output device.";
        return;
    }
}

AudioOutput::~AudioOutput()
{
    if (opusDecoder) {
        opus_decoder_destroy(opusDecoder);
        opusDecoder = nullptr;
    }

    if (audioSink) {
        audioSink->stop();
        delete audioSink;
        audioSink = nullptr;
    }
}

void AudioOutput::addData(const QByteArray &data)
{
    QMutexLocker locker(&mutex);
    dataQueue.enqueue(data);
    locker.unlock();

    // Process the data immediately
    play();
}

void AudioOutput::play()
{
    QMutexLocker locker(&mutex);
    if (dataQueue.isEmpty()) {
        return;
    }

    QByteArray encodedData = dataQueue.dequeue();
    locker.unlock();

    if (!opusDecoder) {
        qCritical() << "Opus decoder is not initialized.";
        return;
    }

    const unsigned char *opusData = reinterpret_cast<const unsigned char *>(encodedData.constData());
    int opusDataSize = encodedData.size();

    int maxFrameSize = opusFrameSize * audioFormat.channelCount();
    QByteArray pcmData(maxFrameSize * sizeof(opus_int16), 0);

    int decodedSamples = opus_decode(
        opusDecoder,
        opusData,
        opusDataSize,
        reinterpret_cast<opus_int16 *>(pcmData.data()),
        maxFrameSize,
        0
        );

    if (decodedSamples < 0) {
        qCritical() << "Opus decoding failed:" << opus_strerror(decodedSamples);
        return;
    }

    int bytesPerSample = audioFormat.bytesPerSample();
    int bytesToWrite = decodedSamples * audioFormat.channelCount() * bytesPerSample;

    qint64 bytesWritten = outputDevice->write(pcmData.constData(), bytesToWrite);

    if (bytesWritten != bytesToWrite) {
        qWarning() << "Not all bytes were written to the audio output device.";
    } else {
        qDebug() << "Played" << bytesWritten << "bytes of audio.";
    }
}
