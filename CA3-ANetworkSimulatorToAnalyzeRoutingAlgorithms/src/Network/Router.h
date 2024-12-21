#ifndef ROUTER_H
#define ROUTER_H

#include "Node.h"
#include <vector>
#include <QSharedPointer>
#include <QEnableSharedFromThis>
#include "../Port/Port.h"
#include "../DHCPServer/DHCPServer.h"
#include <QSet>
#include <QDateTime>

class UDP;
class TopologyBuilder;
class MetricsCollector;

enum class RoutingProtocol {
    RIP,
    OSPF
};

struct RouteEntry {
    QString destination;
    QString mask;
    QString nextHop;
    int metric;
    RoutingProtocol protocol;
    qint64 lastUpdateTime;
    PortPtr_t learnedFromPort;

    bool isDirect;
    int invalidTimer;
    int holdDownTimer;
    int flushTimer;

    RouteEntry(const QString &dest = "",
               const QString &m = "",
               const QString &nh = "",
               int met = -1,
               RoutingProtocol proto = RoutingProtocol::RIP,
               qint64 time = 0,
               PortPtr_t fromPort = nullptr,
               bool direct = false)
        : destination(dest), mask(m), nextHop(nh), metric(met),
        protocol(proto), lastUpdateTime(time), learnedFromPort(fromPort),
        isDirect(direct), invalidTimer(0), holdDownTimer(0), flushTimer(0) {}
};

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
    void forwardPacket(const PacketPtr_t  &packet);
    void logPortStatuses() const;
    void processPacket(const PacketPtr_t  &packet);
    void setIP(QString IP) { m_ipAddress = IP; }
    QString getAssignedIP();

    void setDHCPServer(QSharedPointer<DHCPServer> dhcpServer);
    QSharedPointer<DHCPServer> getDHCPServer();
    bool isDHCPServer() const;
    QString findBestRoute(const QString &destinationIP) const;
    void addDirectRoute(const QString &destination, const QString &mask);

    void setupDirectNeighborRoutes();
    std::vector<QSharedPointer<Router>> getDirectlyConnectedRouters();
    static void setTopologyBuilder(TopologyBuilder *builder);
    void setMetricsCollector(QSharedPointer<MetricsCollector> collector);
    RouteEntry findBestRoutePath(const QString &destinationIP) const;

signals:
    void routingTableUpdated(int routerId);

public Q_SLOTS:
    void initialize();
    void processDHCPResponse(const PacketPtr_t  &packet);

    // Routing table methods
    void addRoute(const QString &destination, const QString &mask, const QString &nextHop, int metric,
                  RoutingProtocol protocol, PortPtr_t learnedFromPort = nullptr);
    void printRoutingTable() const;

    // RIP specific methods
    void enableRIP();
    void onTick();
    void sendRIPUpdate();
    void processRIPUpdate(const PacketPtr_t  &packet);
    void handleRouteTimeouts();

private:
    std::vector<PortPtr_t> m_ports;
    int m_portCount;
    bool m_hasValidIP;
    QSharedPointer<DHCPServer> m_dhcpServer;
    QSharedPointer<UDP> m_udp;
    QSharedPointer<MetricsCollector> m_metricsCollector;
    QString m_assignedIP;

    QSet<QString> m_seenPackets;
    static TopologyBuilder *s_topologyBuilder;
    QVector<RouteEntry> m_routingTable;

    // RIP-related fields
    const int RIP_UPDATE_INTERVAL = 5;
    const int RIP_ROUTE_TIMEOUT   = 180;
    const int RIP_INFINITY        = 16;
    const int RIP_INVALID_TIMER = 20;
    const int RIP_HOLDOWN_TIMER = 20;
    const int RIP_FLUSH_TIMER = 30;

    qint64 m_lastRIPUpdateTime;
    qint64 m_currentTime;

    void initializePorts();
    bool hasSeenPacket(const PacketPtr_t  &packet);
    void markPacketAsSeen(const PacketPtr_t  &packet);
};

#endif // ROUTER_H
