#ifndef ROUTER_H
#define ROUTER_H

#include <QObject>
#include <QThread>
#include <QSharedPointer>
#include <vector>

#include "../Port/Port.h"

class Router : public QThread
{
    Q_OBJECT

public:
    explicit Router(int id, const QString &ipAddress, QObject *parent = nullptr);
    ~Router();

    int getId() const;
    QString getIPAddress() const;
    PortPtr_t getAvailablePort();

    void run() override; // For future use when activating routers

private:
    int m_id;
    QString m_ipAddress;
    std::vector<PortPtr_t> m_ports;
    int m_portCount;

    void initializePorts();
};

#endif // ROUTER_H
