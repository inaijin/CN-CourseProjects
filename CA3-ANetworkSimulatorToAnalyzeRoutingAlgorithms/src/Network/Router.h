#ifndef ROUTER_H
#define ROUTER_H

#include "Node.h"
#include <vector>
#include <QSharedPointer>
#include <QEnableSharedFromThis>
#include "../Port/Port.h"
#include "../DHCPServer/DHCPServer.h"
#include <QSet>

class UDP;

class Router : public Node, public QEnableSharedFromThis<Router>
{
    Q_OBJECT

public:
    explicit Router(int id, const QString &ipAddress, int portCount = 6, QObject *parent = nullptr);
    ~Router() override;

    PortPtr_t getAvailablePort();
    std::vector<PortPtr_t> getPorts();

    void startRouter();
    void requestIPFromDHCP();
    void forwardPacket(const PacketPtr_t &packet);
    void logPortStatuses() const;
    void processPacket(const PacketPtr_t &packet);

    void setDHCPServer(QSharedPointer<DHCPServer> dhcpServer);
    QSharedPointer<DHCPServer> getDHCPServer();
    bool isDHCPServer() const;

public Q_SLOTS:
    void initialize();
    void processDHCPResponse(const PacketPtr_t &packet);
    QString getAssignedIP();

private:
    std::vector<PortPtr_t> m_ports;
    int m_portCount;
    bool m_hasValidIP;
    QSharedPointer<DHCPServer> m_dhcpServer;
    QSharedPointer<UDP> m_udp;
    QString m_assignedIP;

    QSet<QString> m_seenPackets;

    void initializePorts();

    bool hasSeenPacket(const PacketPtr_t &packet);
    void markPacketAsSeen(const PacketPtr_t &packet);
};

#endif // ROUTER_H
