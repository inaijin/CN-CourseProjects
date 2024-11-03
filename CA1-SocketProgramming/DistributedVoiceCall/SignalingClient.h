#ifndef SIGNALINGCLIENT_H
#define SIGNALINGCLIENT_H

#include <QObject>
#include <QTcpSocket>
#include <QJsonDocument>
#include <QJsonObject>

class SignalingClient : public QObject
{
    Q_OBJECT
public:
    explicit SignalingClient(QObject *parent = nullptr);

    void connectToHost(const QString &host, quint16 port);

    void sendOffer(const QString &sdp);
    void sendCandidate(const QString &candidate, const QString &sdpMid);

signals:
    void connectedToServer();
    void answerReceived(const QString &sdp);
    void candidateReceived(const QString &candidate, const QString &sdpMid);

private slots:
    void onConnected();
    void onReadyRead();
    void onErrorOccurred(QAbstractSocket::SocketError socketError);

private:
    QTcpSocket *socket;
    QByteArray buffer; // Buffer for incoming data
};

#endif // SIGNALINGCLIENT_H
