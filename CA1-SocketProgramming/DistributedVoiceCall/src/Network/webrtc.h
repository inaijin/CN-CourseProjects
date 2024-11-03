#ifndef WEBRTC_H
#define WEBRTC_H

#include <QMap>
#include <QObject>

#include <rtc/rtc.hpp>

class WebRTC : public QObject
{
    Q_OBJECT

public:
    explicit WebRTC(QObject *parent = nullptr);
    virtual ~WebRTC();

    Q_INVOKABLE void init(const QString &id, bool isOfferer = false);
    Q_INVOKABLE void addPeer(const QString &peerId);
    Q_INVOKABLE void generateOfferSDP(const QString &peerId);
    Q_INVOKABLE void generateAnswerSDP(const QString &peerId);
    Q_INVOKABLE void sendAudioFrame(const QString &peerId, const QByteArray &frameData);

    bool isOfferer() const;
    bool hasPeer(const QString &peerId) const;
    void setIsOfferer(bool newIsOfferer);
    void resetIsOfferer();

signals:
    void incomingAudioFrame(const QString &peerId, const QByteArray &frameData);
    void localDescriptionGenerated(const QString &peerID, const QString &sdp);
    void localCandidateGenerated(const QString &peerID, const QString &candidate, const QString &sdpMid);
    void isOffererChanged();
    void gatheringCompleted(const QString &peerID);
    void offerIsReady(const QString &peerID, const QString &description);
    void answerIsReady(const QString &peerID, const QString &description);
    void peerConnected(const QString &peerId);
    void trackOpened(const QString &peerId);
    void peerConnectionStateChanged(const QString &peerId, rtc::PeerConnection::State state);

public slots:
    void setRemoteDescription(const QString &peerID, const QString &sdp);
    void setRemoteCandidate(const QString &peerID, const QString &candidate, const QString &sdpMid);

private:
    QString descriptionToJson(const rtc::Description &description);

    QMap<QString, QList<QPair<QString, QString>>> m_pendingCandidates;
    QMap<QString, std::shared_ptr<rtc::Track>> m_audioTracks;
    void setupAudioTrack(const QString &peerId, std::shared_ptr<rtc::PeerConnection> peerConnection);

private:
    bool                                                m_gatheringCompleted = false;
    bool                                                m_isOfferer = false;
    QString                                             m_localId;
    rtc::Configuration                                  m_config;
    QMap<QString, std::shared_ptr<rtc::PeerConnection>> m_peerConnections;
};

#endif // WEBRTC_H
