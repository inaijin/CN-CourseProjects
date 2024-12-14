#include "PC.h"
#include <QDebug>

PC::PC(int id, const QString &ipAddress, QObject *parent)
    : Node(id, ipAddress, NodeType::PC, parent)
{
    m_port = PortPtr_t::create(nullptr);
    m_port->setPortNumber(1);
    m_port->setRouterIP(m_ipAddress);

    connect(m_port.data(), &Port::packetReceived, this, &PC::receivePacket);

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

void PC::generatePacket()
{
    qDebug() << "PC" << m_id << "is generating a packet.";
    auto packet = QSharedPointer<Packet>::create(PacketType::Data, "Payload", 16);
    m_port->sendPacket(packet);
}

void PC::receivePacket(const PacketPtr_t &packet)
{
    if (!packet) return;
    QString payload = packet->getPayload();
    qDebug() << "PC" << m_id << "received packet with payload:" << payload;

    // Handle received packet as needed
}
