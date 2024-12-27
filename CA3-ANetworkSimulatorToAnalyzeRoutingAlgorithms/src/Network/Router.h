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
#include <QTimer>

class UDP;
class TopologyBuilder;
class MetricsCollector;
class PC;

enum class RoutingProtocol {
    RIP,
    OSPF,
    EBGP,
    ITSELF
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
    bool vip;
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
               bool direct = false,
               bool vip = false)
        : destination(dest), mask(m), nextHop(nh), metric(met),
        protocol(proto), lastUpdateTime(time), learnedFromPort(fromPort),
        isDirect(direct), vip(vip), invalidTimer(0), holdDownTimer(0), flushTimer(0) {}
};

struct OSPFNeighbor {
    QString ipAddress;
    int cost;
    qint64 lastHelloReceived;
};

struct OSPFLSA {
    QString originRouterIP;
    QVector<QString> links;
    qint64 sequenceNumber;
    qint64 age;
};

struct Range {
    int min;
    int max;
    int pcMin;
    int pcMax;
};

constexpr int HELLO_INTERVAL = 1000;

class Router : public Node, public QEnableSharedFromThis<Router>
{
    Q_OBJECT

public:
    explicit Router(int id, const QString &ipAddress, int portCount = 6, QObject *parent = nullptr, bool isBroken = false);
    ~Router() override;

    PortPtr_t getAvailablePort();
    std::vector<PortPtr_t> getPorts();

    void startRouter();
    void requestIPFromDHCP();
    void forwardPacket(const PacketPtr_t  &packet);
    void logPortStatuses() const;
    void processPacket(const PacketPtr_t &packet, const PortPtr_t &incomingPort);
    void setIP(QString IP) { m_ipAddress = IP; }
    QString getAssignedIP();

    void setDHCPServer(QSharedPointer<DHCPServer> dhcpServer);
    QSharedPointer<DHCPServer> getDHCPServer();
    bool isDHCPServer() const;
    QString findBestRoute(const QString &destinationIP) const;
    void addDirectRoute(const QString &destination, const QString &mask);

    void setupDirectNeighborRoutes(RoutingProtocol protocol, int ASId, bool bgp);
    std::vector<QSharedPointer<Router>> getDirectlyConnectedRouters(int ASId, bool bgp);
    static void setTopologyBuilder(TopologyBuilder *builder);
    void setMetricsCollector(QSharedPointer<MetricsCollector> collector);
    RouteEntry findBestRoutePath(const QString &destinationIP) const;

    bool isBroken() { return m_isBroken; }
    void addConnectedPC(QSharedPointer<PC> pc, PortPtr_t port);

    void startTimers();
    void setASNum(int num) { m_ASnum = num; }
    bool isRouterBorder();
    void startEBGP();

signals:
    void routingTableUpdated(int routerId);
    void finished();

public slots:
    void onFinished();
    void sendHelloPackets();

public Q_SLOTS:
    void initialize();
    void processDHCPResponse(const PacketPtr_t  &packet);

    // Routing table methods
    void addRoute(const QString &destination, const QString &mask, const QString &nextHop, int metric,
                  RoutingProtocol protocol, PortPtr_t learnedFromPort = nullptr, bool vip = false);
    void printRoutingTable() const;

    // RIP specific methods
    void enableRIP();
    void onTick();
    void sendRIPUpdate();
    void processRIPUpdate(const PacketPtr_t  &packet);
    void processEBGPUpdate(const PacketPtr_t  &packet);
    void handleRouteTimeouts();

    // OSPF-specific methods
    void enableOSPF();
    void sendOSPFHello();
    void processOSPFHello(const PacketPtr_t &packet);
    void sendLSA();
    void processLSA(const PacketPtr_t &packet, const PortPtr_t &incomingPort);
    void runDijkstra();
    void updateRoutingTable();

private:
    std::vector<PortPtr_t> m_ports;
    int m_portCount;
    int m_ASnum;
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

    // OSPF data structures
    QMap<QString, OSPFNeighbor> m_neighbors;
    QMap<QString, OSPFLSA> m_lsdb;
    QTimer *m_helloTimer;
    QTimer *m_lsaTimer;
    qint64 m_lsaSequenceNumber;

    // Dijkstra algorithm helpers
    QMap<QString, int> m_distance;
    QMap<QString, QString> m_previous;

    const int OSPF_HELLO_INTERVAL = 10;
    const int OSPF_LSA_INTERVAL = 30;
    const int OSPF_LSA_AGE_LIMIT = 120;

    void initializeOSPF();
    void handleLSAExpiration();

    qint64 m_lastRIPUpdateTime;
    qint64 m_currentTime;

    void initializePorts();
    bool hasSeenPacket(const PacketPtr_t  &packet);
    bool m_isBroken;
    void markPacketAsSeen(const PacketPtr_t  &packet);
    std::vector<QSharedPointer<PC>> m_connectedPCs;

    Range getRange(int ASnum) const {
        Range range = {0, 0, 0, 0};
        if (ASnum == 1) {
            range.min = 1;
            range.max = 16;
            range.pcMin = 24;
            range.pcMax = 31;
        } else if (ASnum == 2) {
            range.min = 17;
            range.max = 23;
            range.pcMin = 32;
            range.pcMax = 38;
        }
        return range;
    }
};

#endif // ROUTER_H
