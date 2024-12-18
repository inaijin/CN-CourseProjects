#include "PC.h"
#include <QDebug>
#include <QThread>

PC::PC(int id, const QString &ipAddress, QObject *parent)
    : Node(id, ipAddress, NodeType::PC, parent)
{
    m_port = PortPtr_t::create(this);
    m_port->setPortNumber(1);
    m_port->setRouterIP(m_ipAddress);

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
    if (!m_ipAddress.isEmpty())
    {
        qDebug() << "PC" << m_id << "already has a valid IP:" << m_ipAddress;
        return;
    }

    auto packet = QSharedPointer<Packet>::create(PacketType::Control, QString("DHCP_REQUEST:%1").arg(m_id));
    qDebug() << "PC" << m_id << "sending DHCP request with payload:" << packet->getPayload();
    m_port->sendPacket(packet);
}

void PC::processDHCPResponse(const PacketPtr_t &packet)
{
    if (!packet || packet->getPayload().isEmpty())
    {
        qWarning() << "PC" << m_id << "received an empty or invalid packet.";
        return;
    }

    if (packet->getPayload().contains("DHCP_OFFER"))
    {
        QStringList parts = packet->getPayload().split(":");
        if (parts.size() >= 2)
        {
            m_ipAddress = parts[1];
            qDebug() << "PC" << m_id << "received and assigned IP:" << m_ipAddress;
        }
        else
        {
            qWarning() << "PC" << m_id << "received a malformed DHCP offer:" << packet->getPayload();
        }
    }
    else
    {
        qWarning() << "PC" << m_id << "received an unknown packet type:" << packet->getPayload();
    }
}
