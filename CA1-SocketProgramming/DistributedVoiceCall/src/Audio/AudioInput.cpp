#include "AudioInput.h"
#include <QDebug>

AudioInput::AudioInput(QObject *parent)
    : QIODevice(parent),
    audioSource(nullptr),
    inputDevice(nullptr),
    opusEncoder(nullptr),
    opusFrameSize(0)
{
    audioFormat.setSampleRate(48000);
    audioFormat.setChannelCount(1);
    audioFormat.setSampleFormat(QAudioFormat::Int16);

    initializeOpusEncoder();
}

AudioInput::~AudioInput()
{
    stop();

    if (opusEncoder) {
        opus_encoder_destroy(opusEncoder);
        opusEncoder = nullptr;
    }
}

void AudioInput::initializeOpusEncoder()
{
    int error;
    opusEncoder = opus_encoder_create(
        audioFormat.sampleRate(),
        audioFormat.channelCount(),
        OPUS_APPLICATION_VOIP,
        &error
        );

    if (error != OPUS_OK) {
        qCritical() << "Failed to create Opus encoder:" << opus_strerror(error);
        opusEncoder = nullptr;
        return;
    }

    int frameDurationMs = 20;
    opusFrameSize = (audioFormat.sampleRate() / 1000) * frameDurationMs;
}

void AudioInput::start()
{
    if (audioSource) {
        qWarning() << "Audio source is already started.";
        return;
    }

    audioSource = new QAudioSource(audioFormat, this);
    inputDevice = audioSource->start();

    if (!inputDevice) {
        qCritical() << "Failed to start audio input device.";
        return;
    }

    connect(inputDevice, &QIODevice::readyRead, this, &AudioInput::processAudioBuffer);

    open(QIODevice::WriteOnly);
}

void AudioInput::stop()
{
    if (audioSource) {
        audioSource->stop();
        delete audioSource;
        audioSource = nullptr;
    }

    if (inputDevice) {
        inputDevice = nullptr;
    }

    close();
}

void AudioInput::processAudioBuffer()
{
    if (!inputDevice) {
        return;
    }

    QByteArray data = inputDevice->readAll();
    if (data.isEmpty()) {
        return;
    }

    write(data.constData(), data.size());
}

qint64 AudioInput::readData(char *data, qint64 maxlen)
{
    Q_UNUSED(data)
    Q_UNUSED(maxlen)

    return -1;
}

qint64 AudioInput::writeData(const char *data, qint64 len)
{
    if (!opusEncoder) {
        qCritical() << "Opus encoder is not initialized.";
        return -1;
    }

    audioBuffer.append(data, len);
    qDebug() << "Audio buffer size after append:" << audioBuffer.size() << "bytes";

    int bytesPerSample = audioFormat.bytesPerSample();
    int channelCount = audioFormat.channelCount();
    int frameBytes = opusFrameSize * bytesPerSample * channelCount;

    while (audioBuffer.size() >= frameBytes) {
        QByteArray pcmFrame = audioBuffer.left(frameBytes);
        audioBuffer.remove(0, frameBytes);

        qDebug() << "Encoding a frame of size:" << frameBytes << "bytes";

        const opus_int16 *pcmData = reinterpret_cast<const opus_int16 *>(pcmFrame.constData());
        unsigned char opusData[4000]; // Maximum packet size for Opus

        int encodedBytes = opus_encode(
            opusEncoder,
            pcmData,
            opusFrameSize,
            opusData,
            sizeof(opusData)
            );

        if (encodedBytes < 0) {
            qCritical() << "Opus encoding failed:" << opus_strerror(encodedBytes);
            continue;
        } else {
            qDebug() << "Encoded" << encodedBytes << "bytes of Opus data.";
        }

        QByteArray encodedFrame(reinterpret_cast<const char *>(opusData), encodedBytes);
        emit encodedDataReady(encodedFrame);
    }
    return len;
}
