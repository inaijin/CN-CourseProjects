#ifndef PC_H
#define PC_H

#include "Node.h"
#include <QSharedPointer>
#include "../Port/Port.h"
#include "../MetricsCollector/MetricsCollector.h"

class PC : public Node
{
    Q_OBJECT

public:
    explicit PC(int id, const QString &ipAddress, QObject *parent = nullptr);
    ~PC() override;

    PortPtr_t getPort();

    QString getIpAddress() const;

    void setMetricsCollector(QSharedPointer<MetricsCollector> collector);

signals:
    void packetSent(const PacketPtr_t &data);

public Q_SLOTS:
    void initialize();
    void generatePacket();
    void requestIPFromDHCP();
    void processPacket(const PacketPtr_t &packet);

private:
    PortPtr_t m_port;
    QSharedPointer<MetricsCollector> m_metricsCollector;
};

#endif // PC_H
