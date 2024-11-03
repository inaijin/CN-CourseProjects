#include "webrtc.h"
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>

WebRTC::WebRTC(QObject *parent)
    : QObject{parent} {
    qDebug() << "WebRTC object created";
}

WebRTC::~WebRTC() {
    qDebug() << "WebRTC object destroyed";
}

void WebRTC::init(const QString &id, bool isOfferer) {
    qDebug() << "Initializing WebRTC with ID:" << id << "Is offerer:" << isOfferer;

    m_localId = id;
    m_isOfferer = isOfferer;

    // Set up the ICE configuration
    m_config = rtc::Configuration();

    // Clear peer connections for new setup
    m_peerConnections.clear();
    qDebug() << "Cleared existing peer connections";

    Q_EMIT isOffererChanged();
}

void WebRTC::addPeer(const QString &peerId) {
    qDebug() << "Adding peer with ID:" << peerId;

    // Create and add a new peer connection
    auto peerConnection = std::make_shared<rtc::PeerConnection>(m_config);
    m_peerConnections[peerId] = peerConnection;

    // Add any pending candidates
    if (m_pendingCandidates.contains(peerId)) {
        for (const auto &candidatePair : m_pendingCandidates[peerId]) {
            rtc::Candidate rtcCandidate(candidatePair.first.toStdString(), candidatePair.second.toStdString());
            peerConnection->addRemoteCandidate(rtcCandidate);
            qDebug() << "Added pending candidate for peer ID:" << peerId;
        }
        m_pendingCandidates.remove(peerId);
    }

    // Set up callbacks for the peer connection
    peerConnection->onLocalDescription([this, peerId](const rtc::Description &description) {
        qDebug() << "Local description generated for peer ID:" << peerId;
        QString localSdp = descriptionToJson(description);

        if (description.type() == rtc::Description::Type::Offer) {
            Q_EMIT offerIsReady(peerId, localSdp);
        } else if (description.type() == rtc::Description::Type::Answer) {
            Q_EMIT answerIsReady(peerId, localSdp);
        }
    });

    peerConnection->onLocalCandidate([this, peerId](rtc::Candidate candidate) {
        qDebug() << "Local candidate generated for peer ID:" << peerId;
        Q_EMIT localCandidateGenerated(peerId, QString::fromStdString(candidate.candidate()), QString::fromStdString(candidate.mid()));
    });

    peerConnection->onStateChange([this, peerId](rtc::PeerConnection::State state) {
        qDebug() << "Peer connection state changed for peer ID:" << peerId << "State:" << static_cast<int>(state);

        // Emit the state change signal
        Q_EMIT peerConnectionStateChanged(peerId, state);

        if (state == rtc::PeerConnection::State::Connected) {
            Q_EMIT peerConnected(peerId);
        }
    });

    peerConnection->onGatheringStateChange([this, peerId](rtc::PeerConnection::GatheringState state) {
        qDebug() << "Gathering state changed for peer ID:" << peerId << "State:" << static_cast<int>(state);
        if (state == rtc::PeerConnection::GatheringState::Complete) {
            m_gatheringCompleted = true;
            Q_EMIT gatheringCompleted(peerId);
        }
    });

    // Setup audio track
    setupAudioTrack(peerId, peerConnection);
}

void WebRTC::setupAudioTrack(const QString &peerId, std::shared_ptr<rtc::PeerConnection> peerConnection) {
    // Create an audio media description
    rtc::Description::Audio media("audio", rtc::Description::Direction::SendRecv);
    media.addOpusCodec(111); // Payload type for Opus
    media.addSSRC(12345, "audio-send"); // Add SSRC
    media.setBitrate(48000); // Set bitrate

    // Add the audio track to the peer connection
    auto audioTrack = peerConnection->addTrack(media);
    m_audioTracks[peerId] = audioTrack;

    // Set up RTP sessions
    auto rtcpSession = std::make_shared<rtc::RtcpReceivingSession>();
    audioTrack->setMediaHandler(rtcpSession);

    // Handle incoming RTP packets
    audioTrack->onMessage([this, peerId](rtc::message_variant data) {
        qDebug() << "onMessage callback triggered for peer ID:" << peerId;

        if (std::holds_alternative<rtc::binary>(data)) {
            auto rtpPacket = std::get<rtc::binary>(data);
            qDebug() << "Received RTP packet size:" << rtpPacket.size();

            if (rtpPacket.size() < sizeof(rtc::RtpHeader)) {
                qWarning() << "Received data too small to be RTP packet";
                return;
            }

            const rtc::RtpHeader* rtpHeader = reinterpret_cast<const rtc::RtpHeader*>(rtpPacket.data());
            qDebug() << "RTP header payload type:" << rtpHeader->payloadType();

            QByteArray payload(reinterpret_cast<const char*>(rtpPacket.data() + sizeof(rtc::RtpHeader)), rtpPacket.size() - sizeof(rtc::RtpHeader));
            qDebug() << "Payload size:" << payload.size();

            Q_EMIT incomingAudioFrame(peerId, payload);
        } else {
            qWarning() << "Received non-RTP data";
        }
    }, nullptr);

    // Handle track open event
    audioTrack->onOpen([this, peerId]() {
        qDebug() << "Audio track opened for peer ID:" << peerId;
        Q_EMIT trackOpened(peerId);
    });
}

void WebRTC::generateOfferSDP(const QString &peerId) {
    qDebug() << "Generating offer SDP for peer ID:" << peerId;
    if (m_peerConnections.contains(peerId)) {
        auto peerConnection = m_peerConnections[peerId];
        peerConnection->setLocalDescription();
    } else {
        qWarning() << "No peer connection found for peer ID" << peerId;
    }
}

void WebRTC::generateAnswerSDP(const QString &peerId) {
    qDebug() << "Generating answer SDP for peer ID:" << peerId;
    if (m_peerConnections.contains(peerId)) {
        auto peerConnection = m_peerConnections[peerId];
        peerConnection->setLocalDescription();
    } else {
        qWarning() << "No peer connection found for peer ID" << peerId;
    }
}

void WebRTC::sendAudioFrame(const QString &peerId, const QByteArray &frameData) {
    try {
        if (!m_audioTracks.contains(peerId)) {
            qWarning() << "No audio track found for peer" << peerId;
            return;
        }

        auto audioTrack = m_audioTracks[peerId];
        if (audioTrack->isOpen()) {
            // Initialize sequence number and timestamp per track
            static QMap<QString, uint16_t> sequenceNumbers;
            static QMap<QString, uint32_t> timestamps;
            static const uint32_t ssrc = 12345; // Should be unique per stream

            uint16_t &sequenceNumber = sequenceNumbers[peerId];
            uint32_t &timestamp = timestamps[peerId];

            // Construct RTP header
            rtc::RtpHeader rtpHeader;
            rtpHeader.setPayloadType(111); // Opus payload type
            rtpHeader.setSeqNumber(sequenceNumber++);
            rtpHeader.setTimestamp(timestamp);
            rtpHeader.setSsrc(ssrc);

            // Increment timestamp based on Opus frame size
            int frameDurationMs = 20; // Opus frame duration
            uint32_t samplesPerFrame = (48000 / 1000) * frameDurationMs; // 960 samples per frame at 48kHz
            timestamp += samplesPerFrame;

            // Construct RTP packet
            QByteArray rtpPacket(reinterpret_cast<const char*>(&rtpHeader), sizeof(rtc::RtpHeader));
            rtpPacket.append(frameData);

            // Send RTP packet over the track
            rtc::binary data(rtpPacket.size());
            std::memcpy(data.data(), rtpPacket.constData(), rtpPacket.size());
            audioTrack->send(data);
        } else {
            qWarning() << "Audio track is not open for peer" << peerId;
        }
    } catch (const std::exception &e) {
        qCritical() << "Exception in sendAudioFrame:" << e.what();
    } catch (...) {
        qCritical() << "Unknown exception in sendAudioFrame.";
    }
}

void WebRTC::setRemoteDescription(const QString &peerID, const QString &sdp) {
    qDebug() << "Setting remote SDP for peer ID:" << peerID;
    if (m_peerConnections.contains(peerID)) {
        auto peerConnection = m_peerConnections[peerID];

        QJsonDocument doc = QJsonDocument::fromJson(sdp.toUtf8());
        if (!doc.isObject()) {
            qWarning() << "Invalid SDP JSON format";
            return;
        }
        QJsonObject json = doc.object();
        QString typeStr = json["type"].toString();
        QString sdpBase64 = json["sdp"].toString();
        if (typeStr.isEmpty() || sdpBase64.isEmpty()) {
            qWarning() << "SDP JSON missing type or sdp field";
            return;
        }

        QByteArray sdpData = QByteArray::fromBase64(sdpBase64.toLatin1());
        QString sdpStr = QString::fromUtf8(sdpData);

        rtc::Description::Type descType;
        if (typeStr == "offer")
            descType = rtc::Description::Type::Offer;
        else if (typeStr == "answer")
            descType = rtc::Description::Type::Answer;
        else {
            qWarning() << "Unknown SDP type:" << typeStr;
            return;
        }

        rtc::Description remoteDescription(sdpStr.toStdString(), descType);
        peerConnection->setRemoteDescription(remoteDescription);
        qDebug() << "Remote SDP set for peer ID:" << peerID << "Type:" << typeStr;
    } else {
        qWarning() << "No peer connection found for peer ID" << peerID;
    }
}

void WebRTC::setRemoteCandidate(const QString &peerID, const QString &candidate, const QString &sdpMid) {
    qDebug() << "Adding remote candidate for peer ID:" << peerID;
    if (m_peerConnections.contains(peerID)) {
        auto peerConnection = m_peerConnections[peerID];
        rtc::Candidate rtcCandidate(candidate.toStdString(), sdpMid.toStdString());
        peerConnection->addRemoteCandidate(rtcCandidate);
        qDebug() << "Remote candidate added for peer ID:" << peerID;
    } else {
        qWarning() << "No peer connection found for peer ID" << peerID << ". Storing candidate for later.";
        m_pendingCandidates[peerID].append({candidate, sdpMid});
    }
}

QString WebRTC::descriptionToJson(const rtc::Description &description) {
    qDebug() << "Converting rtc::Description to JSON format";
    QJsonObject json;
    QString typeStr = QString::fromStdString(rtc::Description::typeToString(description.type()));
    json["type"] = typeStr;

    QByteArray sdpData = QByteArray::fromStdString(description.generateSdp());
    QString sdpBase64 = QString::fromLatin1(sdpData.toBase64());
    json["sdp"] = sdpBase64;

    QJsonDocument doc(json);
    return QString::fromUtf8(doc.toJson(QJsonDocument::Compact));
}

bool WebRTC::hasPeer(const QString &peerId) const {
    return m_peerConnections.contains(peerId);
}

bool WebRTC::isOfferer() const {
    return m_isOfferer;
}

void WebRTC::setIsOfferer(bool newIsOfferer) {
    if (m_isOfferer != newIsOfferer) {
        m_isOfferer = newIsOfferer;
        Q_EMIT isOffererChanged();
    }
}

void WebRTC::resetIsOfferer() {
    setIsOfferer(false);
}
