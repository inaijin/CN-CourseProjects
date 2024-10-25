#include <QDebug>
#include <QJsonObject>
#include <QJsonDocument>

#include "webrtc.h"

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

    // Emit signal if offerer state changed
    Q_EMIT isOffererChanged();
}

void WebRTC::addPeer(const QString &peerId) {
    qDebug() << "Adding peer with ID:" << peerId;

    // Create and add a new peer connection
    auto newPeer = std::make_shared<rtc::PeerConnection>(m_config);
    m_peerConnections[peerId] = newPeer;
    qDebug() << "Created new peer connection for peer ID:" << peerId;

    // Add any pending candidates
    if (m_pendingCandidates.contains(peerId)) {
        for (const auto &candidatePair : m_pendingCandidates[peerId]) {
            rtc::Candidate rtcCandidate(candidatePair.first.toStdString(), candidatePair.second.toStdString());
            newPeer->addRemoteCandidate(rtcCandidate);
            qDebug() << "Added pending candidate for peer ID:" << peerId;
        }
        m_pendingCandidates.remove(peerId);
    }

    // Set up a callback for when the local description is generated
    newPeer->onLocalDescription([this, peerId](const rtc::Description &description) {
        qDebug() << "Local description generated for peer ID:" << peerId;
        QString localSdp = descriptionToJson(description);

        if (description.type() == rtc::Description::Type::Offer) {
            qDebug() << "Emitting offerIsReady signal for peer ID:" << peerId;
            Q_EMIT offerIsReady(peerId, localSdp);
        } else if (description.type() == rtc::Description::Type::Answer) {
            qDebug() << "Emitting answerIsReady signal for peer ID:" << peerId;
            Q_EMIT answerIsReady(peerId, localSdp);
        }
    });

    // Set up a callback for handling local ICE candidates
    newPeer->onLocalCandidate([this, peerId](rtc::Candidate candidate) {
        qDebug() << "Local candidate generated for peer ID:" << peerId;
        Q_EMIT localCandidateGenerated(peerId, QString::fromStdString(candidate.candidate()), QString::fromStdString(candidate.mid()));
    });

    // Set up a callback for when the state of the peer connection changes
    newPeer->onStateChange([this, peerId](rtc::PeerConnection::State state) {
        qDebug() << "Peer connection state changed for peer ID:" << peerId << "State:" << static_cast<int>(state);

        switch (state) {
        case rtc::PeerConnection::State::New:
            qDebug() << "Peer connection state: New";
            break;
        case rtc::PeerConnection::State::Connecting:
            qDebug() << "Peer connection state: Connecting";
            break;
        case rtc::PeerConnection::State::Connected:
            qDebug() << "Peer connection state: Connected";
            Q_EMIT peerConnected(peerId);
            break;
        case rtc::PeerConnection::State::Disconnected:
            qDebug() << "Peer connection state: Disconnected";
            break;
        case rtc::PeerConnection::State::Failed:
            qDebug() << "Peer connection state: Failed";
            break;
        case rtc::PeerConnection::State::Closed:
            qDebug() << "Peer connection state: Closed";
            break;
        }
    });

    // Set up a callback for monitoring the gathering state
    newPeer->onGatheringStateChange([this, peerId](rtc::PeerConnection::GatheringState state) {
        qDebug() << "Gathering state changed for peer ID:" << peerId << "State:" << static_cast<int>(state);
        if (state == rtc::PeerConnection::GatheringState::Complete) {
            m_gatheringCompleted = true;
            Q_EMIT gatheringCompleted(peerId);
        }
    });

    // Set up data channel
    std::shared_ptr<rtc::DataChannel> dataChannel;
    if (m_isOfferer) {
        dataChannel = newPeer->createDataChannel("data");
        setupDataChannel(peerId, dataChannel);
    }

    // Set up a callback for handling incoming data channels
    newPeer->onDataChannel([this, peerId](std::shared_ptr<rtc::DataChannel> dataChannel) {
        qDebug() << "Data channel received for peer ID:" << peerId;
        setupDataChannel(peerId, dataChannel);
    });
}

void WebRTC::setupDataChannel(const QString &peerId, std::shared_ptr<rtc::DataChannel> dataChannel) {
    dataChannel->onOpen([this, peerId, dataChannel]() {
        qDebug() << "Data channel open for peer ID:" << peerId;
        m_dataChannels[peerId] = dataChannel;
        Q_EMIT trackOpened(peerId);
    });

    dataChannel->onClosed([this, peerId]() {
        qDebug() << "Data channel closed for peer ID:" << peerId;
        m_dataChannels.remove(peerId);
    });

    // Handle incoming messages as tracks
    dataChannel->onMessage([this, peerId](rtc::message_variant data) {
        if (std::holds_alternative<rtc::binary>(data)) {
            const rtc::binary &binaryData = std::get<rtc::binary>(data);
            QByteArray trackData(reinterpret_cast<const char *>(binaryData.data()), binaryData.size());
            qDebug() << "Incoming track data from peer ID:" << peerId << "Data size:" << trackData.size();
            Q_EMIT incomingTrack(peerId, trackData);
        } else if (std::holds_alternative<std::string>(data)) {
            const std::string &stringData = std::get<std::string>(data);
            QByteArray trackData = QByteArray::fromStdString(stringData);
            qDebug() << "Incoming track string data from peer ID:" << peerId << "Data size:" << trackData.size();
            Q_EMIT incomingTrack(peerId, trackData);
        }
    });
}

void WebRTC::generateOfferSDP(const QString &peerId) {
    qDebug() << "Generating offer SDP for peer ID:" << peerId;
    if (m_peerConnections.contains(peerId)) {
        auto peerConnection = m_peerConnections[peerId];
        peerConnection->setLocalDescription();
        qDebug() << "Offer SDP set for peer ID:" << peerId;
    } else {
        qWarning() << "No peer connection found for peer ID" << peerId;
    }
}

void WebRTC::generateAnswerSDP(const QString &peerId) {
    qDebug() << "Generating answer SDP for peer ID:" << peerId;
    if (m_peerConnections.contains(peerId)) {
        auto peerConnection = m_peerConnections[peerId];
        peerConnection->setLocalDescription();
        qDebug() << "Answer SDP set for peer ID:" << peerId;
    } else {
        qWarning() << "No peer connection found for peer ID" << peerId;
    }
}

void WebRTC::addTrack(const QString &peerId, const QByteArray &trackData) {
    qDebug() << "Adding track to peer ID:" << peerId << "Track data size:" << trackData.size();
    if (!m_dataChannels.contains(peerId)) {
        qWarning() << "No data channel found for peer" << peerId;
        return;
    }

    auto dataChannel = m_dataChannels[peerId];
    if (dataChannel->isOpen()) {
        rtc::binary data(trackData.size());
        std::memcpy(data.data(), trackData.constData(), trackData.size());
        dataChannel->send(data);
        qDebug() << "Track data sent to peer ID:" << peerId;
    } else {
        qWarning() << "Data channel is not open for peer" << peerId;
    }
}

// Set the remote SDP description
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

// Add remote ICE candidates
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

// Convert rtc::Description to JSON format
QString WebRTC::descriptionToJson(const rtc::Description &description) {
    qDebug() << "Converting rtc::Description to JSON format";
    QJsonObject json;
    QString typeStr = QString::fromStdString(rtc::Description::typeToString(description.type()));
    json["type"] = typeStr;

    QByteArray sdpData = QByteArray::fromStdString(description.generateSdp());
    QString sdpBase64 = QString::fromLatin1(sdpData.toBase64());
    json["sdp"] = sdpBase64;

    QJsonDocument doc(json);
    qDebug() << "Generated JSON description:" << QString::fromUtf8(doc.toJson(QJsonDocument::Compact));
    return QString::fromUtf8(doc.toJson(QJsonDocument::Compact));
}

bool WebRTC::hasPeer(const QString &peerId) const {
    return m_peerConnections.contains(peerId);
}

// Retrieves the current bit rate
int WebRTC::bitRate() const
{
    return m_bitRate;
}

// Set a new bit rate and emit the bitRateChanged signal
void WebRTC::setBitRate(int newBitRate)
{
    if (m_bitRate != newBitRate) {
        m_bitRate = newBitRate;
        m_audio.setBitrate(m_bitRate);
        Q_EMIT bitRateChanged();
    }
}

// Reset the bit rate to its default value
void WebRTC::resetBitRate()
{
    setBitRate(48000);
}

// Sets a new payload type and emit the payloadTypeChanged signal
void WebRTC::setPayloadType(int newPayloadType)
{
    if (m_payloadType != newPayloadType) {
        m_payloadType = newPayloadType;
        m_audio.addOpusCodec(m_payloadType);
        Q_EMIT payloadTypeChanged();
    }
}

// Resets the payload type to its default value
void WebRTC::resetPayloadType()
{
    setPayloadType(111);
}

// Retrieve the current SSRC value
rtc::SSRC WebRTC::ssrc() const
{
    return m_ssrc;
}

// Set a new SSRC and emit the ssrcChanged signal
void WebRTC::setSsrc(rtc::SSRC newSsrc)
{
    if (m_ssrc != newSsrc) {
        m_ssrc = newSsrc;
        Q_EMIT ssrcChanged();
    }
}

// Reset the SSRC to its default value
void WebRTC::resetSsrc()
{
    setSsrc(2);
}

int WebRTC::payloadType() const
{
    return m_payloadType;
}

bool WebRTC::isOfferer() const
{
    return m_isOfferer;
}

void WebRTC::setIsOfferer(bool newIsOfferer)
{
    if (m_isOfferer != newIsOfferer) {
        m_isOfferer = newIsOfferer;
        Q_EMIT isOffererChanged();
    }
}

void WebRTC::resetIsOfferer()
{
    setIsOfferer(false);
}
