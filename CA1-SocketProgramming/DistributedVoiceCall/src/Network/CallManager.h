#ifndef CALLMANAGER_H
#define CALLMANAGER_H

#include <QObject>
#include <QString>

#include "SignalingClient.h"
#include "SignalingServer.h"
#include "WebRTC.h"
#include "../Audio/AudioInput.h"
#include "../Audio/AudioOutput.h"

class CallManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString role READ role WRITE setRole NOTIFY roleChanged)
    Q_PROPERTY(QString host READ host WRITE setHost NOTIFY hostChanged)
    Q_PROPERTY(quint16 port READ port WRITE setPort NOTIFY portChanged)
    Q_PROPERTY(QString status READ status NOTIFY statusChanged)

public:
    explicit CallManager(QObject *parent = nullptr);

    QString role() const;
    void setRole(const QString &newRole);

    QString host() const;
    void setHost(const QString &newHost);

    quint16 port() const;
    void setPort(quint16 newPort);

    QString status() const;

public slots:
    void startCall();
    void endCall();

signals:
    void roleChanged();
    void hostChanged();
    void portChanged();
    void statusChanged();

private:
    void setupCaller();
    void setupCallee();

    QString m_role;
    QString m_host;
    quint16 m_port;
    QString m_status;

    QString peerID;

    SignalingClient *signalingClient = nullptr;
    SignalingServer *signalingServer = nullptr;
    WebRTC webRTC;

    AudioInput *audioInput = nullptr;
    AudioOutput *audioOutput = nullptr;

    bool audioTrackOpen = false;
};

#endif // CALLMANAGER_H
