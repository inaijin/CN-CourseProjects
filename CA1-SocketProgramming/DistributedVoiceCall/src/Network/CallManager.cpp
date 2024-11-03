#include "CallManager.h"
#include <QDebug>

CallManager::CallManager(QObject *parent)
    : QObject(parent),
    m_host("127.0.0.1"),
    m_port(12345),
    peerID("peer"),
    audioTrackOpen(false)
{
}

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
        signalingClient->deleteLater();
        signalingClient = nullptr;
    }
    if (signalingServer) {
        signalingServer->deleteLater();
        signalingServer = nullptr;
    }
    if (audioInput) {
        audioInput->stop();
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
        qDebug() << "SignalingClient connected to server.";
        webRTC.addPeer(peerID);
        webRTC.generateOfferSDP(peerID);
    });

    connect(&webRTC, &WebRTC::offerIsReady, this, [this](const QString &id, const QString &description) {
        Q_UNUSED(id);
        signalingClient->sendOffer(description);
    });

    connect(signalingClient, &SignalingClient::answerReceived, this, [this](const QString &sdp) {
        webRTC.setRemoteDescription(peerID, sdp);
    });

    connect(&webRTC, &WebRTC::localCandidateGenerated, this, [this](const QString &id, const QString &candidate, const QString &sdpMid) {
        Q_UNUSED(id);
        signalingClient->sendCandidate(candidate, sdpMid);
    });

    connect(signalingClient, &SignalingClient::candidateReceived, this, [this](const QString &candidate, const QString &sdpMid) {
        webRTC.setRemoteCandidate(peerID, candidate, sdpMid);
    });

    audioInput = new AudioInput(this);
    audioOutput = new AudioOutput(this);

    connect(&webRTC, &WebRTC::trackOpened, this, [this](const QString &peerId) {
        qDebug() << "Audio track opened for peer ID:" << peerId << ". Starting AudioInput.";
        audioTrackOpen = true;
        audioInput->start();
    });

    connect(&webRTC, &WebRTC::peerConnectionStateChanged, this, [this](const QString &peerId, rtc::PeerConnection::State state) {
        qDebug() << "Peer connection state changed for peer ID:" << peerId << "State:" << static_cast<int>(state);

        if (state == rtc::PeerConnection::State::Disconnected ||
            state == rtc::PeerConnection::State::Failed ||
            state == rtc::PeerConnection::State::Closed) {
            qDebug() << "Peer connection closed or failed. Stopping AudioInput.";
            audioTrackOpen = false;
            audioInput->stop();
        }
    });

    connect(audioInput, &AudioInput::encodedDataReady, this, [this](const QByteArray &encodedData) {
        if (audioTrackOpen) {
            webRTC.sendAudioFrame(peerID, encodedData);
        }
    });

    connect(&webRTC, &WebRTC::incomingAudioFrame, this, [this](const QString &peerId, const QByteArray &frameData) {
        Q_UNUSED(peerId);
        audioOutput->addData(frameData);
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
        qDebug() << "SignalingServer received offer SDP.";
        if (!webRTC.hasPeer(peerID)) {
            webRTC.addPeer(peerID);
        }
        webRTC.setRemoteDescription(peerID, sdp);
        webRTC.generateAnswerSDP(peerID);
    });

    connect(&webRTC, &WebRTC::answerIsReady, this, [this](const QString &id, const QString &description) {
        Q_UNUSED(id);
        signalingServer->sendAnswer(description);
    });

    connect(signalingServer, &SignalingServer::candidateReceived, this, [this](const QString &candidate, const QString &sdpMid) {
        webRTC.setRemoteCandidate(peerID, candidate, sdpMid);
    });

    connect(&webRTC, &WebRTC::localCandidateGenerated, this, [this](const QString &id, const QString &candidate, const QString &sdpMid) {
        Q_UNUSED(id);
        signalingServer->sendCandidate(candidate, sdpMid);
    });

    audioInput = new AudioInput(this);
    audioOutput = new AudioOutput(this);

    connect(&webRTC, &WebRTC::trackOpened, this, [this](const QString &peerId) {
        qDebug() << "Audio track opened for peer ID:" << peerId << ". Starting AudioInput.";
        audioTrackOpen = true;
        audioInput->start();
    });

    connect(&webRTC, &WebRTC::peerConnectionStateChanged, this, [this](const QString &peerId, rtc::PeerConnection::State state) {
        qDebug() << "Peer connection state changed for peer ID:" << peerId << "State:" << static_cast<int>(state);

        if (state == rtc::PeerConnection::State::Disconnected ||
            state == rtc::PeerConnection::State::Failed ||
            state == rtc::PeerConnection::State::Closed) {
            qDebug() << "Peer connection closed or failed. Stopping AudioInput.";
            audioTrackOpen = false;
            audioInput->stop();
        }
    });

    connect(audioInput, &AudioInput::encodedDataReady, this, [this](const QByteArray &encodedData) {
        if (audioTrackOpen) {
            webRTC.sendAudioFrame(peerID, encodedData);
        }
    });

    connect(&webRTC, &WebRTC::incomingAudioFrame, this, [this](const QString &peerId, const QByteArray &frameData) {
        Q_UNUSED(peerId);
        audioOutput->addData(frameData);
    });

    m_status = "Callee initialized";
    emit statusChanged();
}
