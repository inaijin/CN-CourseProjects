#include "SignalingClient.h"
#include <QDebug>

SignalingClient::SignalingClient(QObject *parent)
    : QObject(parent),
    socket(new QTcpSocket(this))
{}

void SignalingClient::connectToHost(const QString &host, quint16 port)
{
    connect(socket, &QTcpSocket::connected, this, &SignalingClient::onConnected);
    connect(socket, &QTcpSocket::readyRead, this, &SignalingClient::onReadyRead);
    connect(socket, &QTcpSocket::errorOccurred, this, &SignalingClient::onErrorOccurred);
    socket->connectToHost(host, port);

    if (socket->state() == QAbstractSocket::UnconnectedState) {
        qWarning() << "SignalingClient failed to initiate connection to host:" << host << "port:" << port;
    }
}

void SignalingClient::onConnected()
{
    qDebug() << "SignalingClient connected to server.";
    emit connectedToServer();
}

void SignalingClient::onReadyRead()
{
    buffer.append(socket->readAll());

    while (true) {
        int index = buffer.indexOf('\n');
        if (index == -1) {
            break;
        }

        QByteArray data = buffer.left(index);
        buffer.remove(0, index + 1);

        qDebug() << "SignalingClient received data:" << data;

        QJsonParseError parseError;
        QJsonDocument doc = QJsonDocument::fromJson(data, &parseError);
        if (doc.isNull()) {
            qWarning() << "Invalid JSON received:" << parseError.errorString();
            continue;
        }

        if (!doc.isObject()) {
            qWarning() << "JSON is not an object";
            continue;
        }

        QJsonObject obj = doc.object();
        QString type = obj["type"].toString();

        if (type == "answer") {
            QString sdp = obj["sdp"].toString();
            qDebug() << "SignalingClient received answer SDP.";
            emit answerReceived(sdp);
        } else if (type == "candidate") {
            QString candidate = obj["candidate"].toString();
            QString sdpMid = obj["sdpMid"].toString();
            qDebug() << "SignalingClient received ICE candidate.";
            emit candidateReceived(candidate, sdpMid);
        } else {
            qWarning() << "Unknown message type received:" << type;
        }
    }
}

void SignalingClient::onErrorOccurred(QAbstractSocket::SocketError socketError)
{
    qCritical() << "SignalingClient socket error:" << socketError << socket->errorString();
}

void SignalingClient::sendOffer(const QString &sdp)
{
    QJsonObject obj;
    obj["type"] = "offer";
    obj["sdp"] = sdp;
    QJsonDocument doc(obj);
    QByteArray data = doc.toJson(QJsonDocument::Compact);
    data.append('\n');
    socket->write(data);
    qDebug() << "SignalingClient sent offer to server:" << data;
}

void SignalingClient::sendCandidate(const QString &candidate, const QString &sdpMid)
{
    QJsonObject obj;
    obj["type"] = "candidate";
    obj["candidate"] = candidate;
    obj["sdpMid"] = sdpMid;
    QJsonDocument doc(obj);
    QByteArray data = doc.toJson(QJsonDocument::Compact);
    data.append('\n');
    socket->write(data);
    qDebug() << "SignalingClient sent candidate to server:" << data;
}
