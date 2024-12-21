#include "PC.h"
#include <QDebug>
#include <QThread>
#include "../Packet/Packet.h"

PC::PC(int id, const QString &ipAddress, QObject *parent)
    : Node(id, ipAddress, NodeType::PC, parent)
{
    m_port = PortPtr_t::create(this);
    m_port->setPortNumber(1);
    m_port->setRouterIP(m_ipAddress);

    connect(m_port.data(), &Port::packetReceived, this, &PC::processPacket);

    qDebug() << "PC initialized: ID =" << m_id << ", IP =" << m_ipAddress;
}

PC::~PC()
{
    qDebug() << "PC destroyed: ID =" << m_id;
}

PortPtr_t PC::getPort()
{
    return m_port;
}

void PC::initialize()
{
    qDebug() << "PC initialized: ID =" << m_id << ", IP =" << m_ipAddress
             << ", running in thread" << (quintptr)QThread::currentThreadId();
}

void PC::generatePacket()
{
    qDebug() << "PC" << m_id << "is generating a packet.";
    auto packet = QSharedPointer<Packet>::create(PacketType::Data, "Payload");
    m_port->sendPacket(packet);
}

void PC::requestIPFromDHCP()
{
    qDebug() << "PC" << m_id << "requesting IP via DHCP.";
    auto packet = QSharedPointer<Packet>::create(PacketType::Control, QString("DHCP_REQUEST:%1").arg(m_id));
    m_port->sendPacket(packet);
}

void PC::processPacket(const PacketPtr_t &packet)
{
    if (!packet) return;

    QString payload = packet->getPayload();
    qDebug() << "PC" << m_id << "received packet with payload:" << payload;

    if (payload.contains("DHCP_OFFER")) {
        // Payload format: DHCP_OFFER:IPADDRESS:clientId
        QStringList parts = payload.split(":");
        if (parts.size() == 3) {
            QString offeredIP = parts[1];
            int clientId = parts[2].toInt();

            if (clientId == m_id) {
                qDebug() << "PC" << m_id << "received DHCP offer:" << offeredIP << "Assigning IP.";
                m_ipAddress = offeredIP;
                qDebug() << "PC" << m_id << "assigned IP:" << m_ipAddress;
            }
        } else {
            qWarning() << "Malformed DHCP_OFFER packet on PC" << m_id << "payload:" << payload;
        }
    } else {
        qDebug() << "PC" << m_id << "received unknown/unsupported packet, dropping it.";
    }
}

QString PC::getIpAddress() const {
    return m_ipAddress;
}

void PC::setMetricsCollector(QSharedPointer<MetricsCollector> collector) {
    m_metricsCollector = collector;
}
