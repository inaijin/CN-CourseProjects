#ifndef ROUTER_H
#define ROUTER_H

#include "Node.h"
#include <vector>
#include <QSharedPointer>
#include "../Port/Port.h"

class Router : public Node
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

Q_SIGNALS:
    void sendDHCPRequest(const PacketPtr_t &packet);
    void receiveIPFromDHCP(const PacketPtr_t &packet);

public Q_SLOTS:
    void processDHCPResponse(const PacketPtr_t &packet);

protected:
    void run() override;

private:
    std::vector<PortPtr_t> m_ports;
    int m_portCount;
    bool m_hasValidIP;

    void initializePorts();
    void processPacket(const PacketPtr_t &packet);
    QString m_assignedIP;
};

#endif // ROUTER_H
