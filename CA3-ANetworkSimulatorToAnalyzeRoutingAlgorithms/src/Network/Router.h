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
    void forwardPacket(const PacketPtr_t &packet);
    void logPortStatuses() const;

protected:
    void run() override;

private:
    std::vector<PortPtr_t> m_ports;
    int m_portCount;

    void initializePorts();
    void processPacket(const PacketPtr_t &packet);
};

#endif // ROUTER_H
