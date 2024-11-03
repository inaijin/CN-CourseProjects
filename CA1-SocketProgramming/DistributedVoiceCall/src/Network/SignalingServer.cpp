#include "SignalingServer.h"
#include <QDebug>

SignalingServer::SignalingServer(QObject *parent)
    : QObject(parent),
    tcpServer(new QTcpServer(this)),
    clientSocket(nullptr)
{
    connect(tcpServer, &QTcpServer::newConnection, this, &SignalingServer::onNewConnection);
}

bool SignalingServer::startListening(quint16 port)
{
    if (!tcpServer->listen(QHostAddress::Any, port)) {
        qCritical() << "Failed to start server:" << tcpServer->errorString();
        return false;
    }
    qDebug() << "SignalingServer listening on port" << port;
    return true;
}

void SignalingServer::onNewConnection()
{
    clientSocket = tcpServer->nextPendingConnection();
    qDebug() << "SignalingServer: New client connected from" << clientSocket->peerAddress().toString() << ":" << clientSocket->peerPort();
    connect(clientSocket, &QTcpSocket::readyRead, this, &SignalingServer::onReadyRead);
    connect(clientSocket, &QTcpSocket::disconnected, this, &SignalingServer::onClientDisconnected);
    connect(clientSocket, &QTcpSocket::errorOccurred, this, &SignalingServer::onErrorOccurred);
}

void SignalingServer::onReadyRead()
{
    buffer.append(clientSocket->readAll());

    while (true) {
        int index = buffer.indexOf('\n');
        if (index == -1) {
            // No complete message yet
            break;
        }

        QByteArray data = buffer.left(index);
        buffer.remove(0, index + 1);

        qDebug() << "SignalingServer received data:" << data;

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

        if (type == "offer") {
            QString sdp = obj["sdp"].toString();
            qDebug() << "SignalingServer received offer SDP.";
            emit offerReceived(sdp);
        } else if (type == "candidate") {
            QString candidate = obj["candidate"].toString();
            QString sdpMid = obj["sdpMid"].toString();
            qDebug() << "SignalingServer received ICE candidate.";
            emit candidateReceived(candidate, sdpMid);
        } else {
            qWarning() << "Unknown message type received:" << type;
        }
    }
}

void SignalingServer::onErrorOccurred(QAbstractSocket::SocketError socketError)
{
    qCritical() << "SignalingServer socket error:" << socketError << clientSocket->errorString();
}

void SignalingServer::onClientDisconnected()
{
    qDebug() << "SignalingServer: Client disconnected.";
    clientSocket->deleteLater();
    clientSocket = nullptr;
}

void SignalingServer::sendAnswer(const QString &sdp)
{
    if (!clientSocket) {
        qWarning() << "No client connected to send answer.";
        return;
    }

    QJsonObject obj;
    obj["type"] = "answer";
    obj["sdp"] = sdp;
    QJsonDocument doc(obj);
    QByteArray data = doc.toJson(QJsonDocument::Compact);
    data.append('\n');
    clientSocket->write(data);
    qDebug() << "SignalingServer sent answer to client:" << data;
}

void SignalingServer::sendCandidate(const QString &candidate, const QString &sdpMid)
{
    if (!clientSocket) {
        qWarning() << "No client connected to send candidate.";
        return;
    }

    QJsonObject obj;
    obj["type"] = "candidate";
    obj["candidate"] = candidate;
    obj["sdpMid"] = sdpMid;
    QJsonDocument doc(obj);
    QByteArray data = doc.toJson(QJsonDocument::Compact);
    data.append('\n');
    clientSocket->write(data);
    qDebug() << "SignalingServer sent candidate to client:" << data;
}
