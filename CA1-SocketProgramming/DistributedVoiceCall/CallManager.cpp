#include "CallManager.h"
#include <QDebug>
#include <QTimer>

CallManager::CallManager(QObject *parent)
    : QObject(parent), m_host("127.0.0.1"), m_port(12345), peerID("peer") {}

QString CallManager::role() const {
    return m_role;
}

void CallManager::setRole(const QString &newRole) {
    if (m_role != newRole) {
        m_role = newRole.toLower();
        emit roleChanged();
    }
}

QString CallManager::host() const {
    return m_host;
}

void CallManager::setHost(const QString &newHost) {
    if (m_host != newHost) {
        m_host = newHost;
        emit hostChanged();
    }
}

quint16 CallManager::port() const {
    return m_port;
}

void CallManager::setPort(quint16 newPort) {
    if (m_port != newPort) {
        m_port = newPort;
        emit portChanged();
    }
}

QString CallManager::status() const {
    return m_status;
}

void CallManager::startCall() {
    if (m_role == "caller") {
        setupCaller();
    } else if (m_role == "callee") {
        setupCallee();
    } else {
        qCritical() << "Invalid role specified. Use 'caller' or 'callee'.";
        return;
    }
}

void CallManager::endCall() {
    if (signalingClient) {
        signalingClient->deleteLater();  // Clean up the signaling client
        signalingClient = nullptr;
    }
    if (signalingServer) {
        signalingServer->deleteLater();  // Clean up the signaling server
        signalingServer = nullptr;
    }
    if (audioInput) {
        audioInput->stop();  // Stop the audio input
        audioInput->deleteLater();
        audioInput = nullptr;
    }
    if (audioOutput) {
        audioOutput->deleteLater();
        audioOutput = nullptr;
    }
    m_status = "Call ended";
    emit statusChanged();
}

void CallManager::setupCaller() {
    qDebug() << "Initializing as caller";
    webRTC.init("caller", true);

    signalingClient = new SignalingClient(this);
    signalingClient->connectToHost(m_host, m_port);

    connect(signalingClient, &SignalingClient::connectedToServer, this, [this]() {
        qDebug() << "Connected to signaling server. Adding peer and generating offer.";
        webRTC.addPeer(peerID);
        webRTC.generateOfferSDP(peerID);
    });

    connect(&webRTC, &WebRTC::offerIsReady, signalingClient, [this](const QString &id, const QString &description) {
        Q_UNUSED(id);
        qDebug() << "Offer is ready. Sending to signaling server.";
        signalingClient->sendOffer(description);
    });

    connect(signalingClient, &SignalingClient::answerReceived, this, [this](const QString &sdp) {
        qDebug() << "Answer received from signaling server.";
        webRTC.setRemoteDescription(peerID, sdp);
    });

    connect(&webRTC, &WebRTC::localCandidateGenerated, signalingClient, [this](const QString &id, const QString &candidate, const QString &sdpMid) {
        Q_UNUSED(id);
        qDebug() << "Local ICE candidate generated. Sending to signaling server.";
        signalingClient->sendCandidate(candidate, sdpMid);
    });

    connect(signalingClient, &SignalingClient::candidateReceived, this, [this](const QString &candidate, const QString &sdpMid) {
        qDebug() << "Remote ICE candidate received from signaling server.";
        webRTC.setRemoteCandidate(peerID, candidate, sdpMid);
    });

    audioInput = new AudioInput(this);
    audioOutput = new AudioOutput(this);

    connect(&webRTC, &WebRTC::trackOpened, this, [this]() {
        qDebug() << "Data channel opened. Starting AudioInput on caller side.";
        audioInput->start();
    });

    connect(audioInput, &AudioInput::encodedDataReady, this, [this](const QByteArray &encodedData) {
        webRTC.addTrack(peerID, encodedData);
    });

    connect(&webRTC, &WebRTC::incomingTrack, this, [this](const QString &peerId, const QByteArray &trackData) {
        Q_UNUSED(peerId);
        audioOutput->addData(trackData);
    });

    m_status = "Caller initialized";
    emit statusChanged();
}

void CallManager::setupCallee() {
    qDebug() << "Initializing as callee";
    webRTC.init("callee", false);

    signalingServer = new SignalingServer(this);
    if (!signalingServer->startListening(m_port)) {
        qCritical() << "Failed to start signaling server on port" << m_port;
        return;
    }

    connect(signalingServer, &SignalingServer::offerReceived, this, [this](const QString &sdp) {
        qDebug() << "Offer received from signaling client.";
        if (!webRTC.hasPeer(peerID)) {
            webRTC.addPeer(peerID);
        }
        webRTC.setRemoteDescription(peerID, sdp);
        webRTC.generateAnswerSDP(peerID);
    });

    connect(&webRTC, &WebRTC::answerIsReady, signalingServer, [this](const QString &id, const QString &description) {
        Q_UNUSED(id);
        qDebug() << "Answer is ready. Sending to signaling client.";
        signalingServer->sendAnswer(description);
    });

    connect(signalingServer, &SignalingServer::candidateReceived, this, [this](const QString &candidate, const QString &sdpMid) {
        qDebug() << "Remote ICE candidate received from signaling client.";
        webRTC.setRemoteCandidate(peerID, candidate, sdpMid);
    });

    connect(&webRTC, &WebRTC::localCandidateGenerated, signalingServer, [this](const QString &id, const QString &candidate, const QString &sdpMid) {
        Q_UNUSED(id);
        qDebug() << "Local ICE candidate generated. Sending to signaling client.";
        signalingServer->sendCandidate(candidate, sdpMid);
    });

    audioInput = new AudioInput(this);
    audioOutput = new AudioOutput(this);

    connect(&webRTC, &WebRTC::trackOpened, this, [this]() {
        qDebug() << "Data channel opened. Starting AudioInput on callee side.";
        audioInput->start();
    });

    connect(audioInput, &AudioInput::encodedDataReady, this, [this](const QByteArray &encodedData) {
        webRTC.addTrack(peerID, encodedData);
    });

    connect(&webRTC, &WebRTC::incomingTrack, this, [this](const QString &peerId, const QByteArray &trackData) {
        Q_UNUSED(peerId);
        audioOutput->addData(trackData);

        // // Add a delay of 100ms before sending back the data to create echo
        // QTimer::singleShot(100, [trackData, this]() {
        //     webRTC.addTrack(peerID, trackData);
        //     qDebug() << "Echoed back audio data to peer ID:" << peerID;
        // });
    });

    m_status = "Callee initialized";
    emit statusChanged();
}
