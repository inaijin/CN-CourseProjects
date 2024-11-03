#ifndef SIGNALINGSERVER_H
#define SIGNALINGSERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QJsonDocument>
#include <QJsonObject>

class SignalingServer : public QObject
{
    Q_OBJECT
public:
    explicit SignalingServer(QObject *parent = nullptr);
    bool startListening(quint16 port);

    // Declare as public functions
    void sendAnswer(const QString &sdp);
    void sendCandidate(const QString &candidate, const QString &sdpMid);

signals:
    void offerReceived(const QString &sdp);
    void candidateReceived(const QString &candidate, const QString &sdpMid);

private slots:
    void onNewConnection();
    void onReadyRead();
    void onClientDisconnected();
    void onErrorOccurred(QAbstractSocket::SocketError socketError);

private:
    QTcpServer *tcpServer;
    QTcpSocket *clientSocket;
    QByteArray buffer;
};

#endif // SIGNALINGSERVER_H
