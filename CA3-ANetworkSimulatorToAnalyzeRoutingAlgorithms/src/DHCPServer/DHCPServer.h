#ifndef DHCPSERVER_H
#define DHCPSERVER_H

#include <QObject>
#include <QSharedPointer>
#include "../Packet/Packet.h"
#include "../Port/Port.h"

class Router;

class DHCPServer : public QObject
{
    Q_OBJECT

public:
    explicit DHCPServer(int asId, const QSharedPointer<Router> &router, QObject *parent = nullptr);
    ~DHCPServer() override;

    void receivePacket(const PacketPtr_t &packet);
    void tick(int currentTime);

Q_SIGNALS:
    void broadcastPacket(const PacketPtr_t &packet);

private:
    struct DHCPLease {
        QString ipAddress;
        int clientId;
        int leaseExpirationTime;
    };

    void assignIP(const PacketPtr_t &packet);
    void sendOffer(const DHCPLease &lease);
    void reclaimExpiredLeases();

    int m_asId;
    QSharedPointer<Port> m_port;  // For directly assigned ports
    QSharedPointer<Router> m_router;  // For router-based initialization
    QVector<DHCPLease> m_leases;
    QString m_ipPrefix;
    int m_nextAvailableId;
    int m_currentTime;

    static const int LEASE_DURATION = 300; // Lease duration in seconds
};

#endif // DHCPSERVER_H
