#ifndef CALLMANAGER_H
#define CALLMANAGER_H

#include <QObject>
#include <QString>
#include "webrtc.h"
#include "AudioInput.h"
#include "AudioOutput.h"
#include "SignalingServer.h"
#include "SignalingClient.h"

class CallManager : public QObject {
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

    Q_INVOKABLE void startCall();
    Q_INVOKABLE void endCall();

signals:
    void roleChanged();
    void hostChanged();
    void portChanged();
    void statusChanged();

private:
    QString m_role;
    QString m_host;
    quint16 m_port;
    QString m_status;

    WebRTC webRTC;
    SignalingClient *signalingClient;
    SignalingServer *signalingServer;
    AudioInput *audioInput;
    AudioOutput *audioOutput;
    QString peerID;

    void setupCaller();
    void setupCallee();
};

#endif // CALLMANAGER_H
