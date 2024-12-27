#include "PC.h"
#include <QDebug>
#include <QThread>
#include "../Packet/Packet.h"
#include "../MACAddress/MACADdressGenerator.h"

PC::PC(int id, const QString &ipAddress, QObject *parent)
    : Node(id, ipAddress, NodeType::PC, parent)
{
    m_port = PortPtr_t::create(this);
    m_port->setPortNumber(1);
    m_port->setRouterIP(m_ipAddress->getIp());

    connect(m_port.data(), &Port::packetReceived, this, &PC::processPacket);

    QSharedPointer<MACAddressGenerator> generator = QSharedPointer<MACAddressGenerator>::create();
    m_macAddress = generator->generate();

    qDebug() << "PC initialized: ID =" << m_id << ", IP =" << m_ipAddress->getIp();
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
    qDebug() << "PC initialized: ID =" << m_id << ", IP =" << m_ipAddress->getIp()
             << ", running in thread" << (quintptr)QThread::currentThreadId();
}

void PC::generatePacket()
{
    qDebug() << "PC" << m_id << "is generating a packet.";
    auto packet = QSharedPointer<Packet>::create(PacketType::Data, "Payload");
    m_port->sendPacket(packet);

    emit packetSent(packet);
}

void PC::requestIPFromDHCP()
{
    qDebug() << "PC" << m_id << "requesting IP via DHCP.";
    auto packet = QSharedPointer<Packet>::create(PacketType::Control, QString("DHCP_REQUEST:%1").arg(m_id));
    m_port->sendPacket(packet);

    emit packetSent(packet);
}

void PC::processPacket(const PacketPtr_t &packet)
{
    if (!packet) return;

    QString payload = packet->getPayload();
    qDebug() << "PC" << m_id << "received packet with payload:" << payload;

    if (packet->getType() == PacketType::Data) {
        QStringList parts = payload.split(":");
        if (parts.size() >= 3 && parts.at(0) == "Data") {
            QString destinationIP = parts.at(1);
            QString actualPayload = parts.at(2);

            if (destinationIP == m_ipAddress->getIp()) {
                qDebug() << "PC" << m_id << "received data packet intended for itself.";

                // Record packet reception metrics
                if (m_metricsCollector) {
                    m_metricsCollector->recordPacketReceived(packet->getPath());
                }

                qDebug() << "PC" << m_id << "processing payload:" << actualPayload;
            }
            else {
                qDebug() << "PC" << m_id << "received data packet not intended for it. Dropping.";

                // Record packet drop
                if (m_metricsCollector) {
                    m_metricsCollector->recordPacketDropped();
                }
            }
        }
        else {
            qWarning() << "Malformed Data packet on PC" << m_id << "payload:" << payload;
            if (m_metricsCollector) {
                m_metricsCollector->recordPacketDropped();
            }
        }
    }
    else if (payload.contains("DHCP_OFFER")) {
        // DHCP_OFFER handling logic
        QStringList parts = payload.split(":");
        if (parts.size() == 3) {
            QString offeredIP = parts.at(1);
            int clientId = parts.at(2).toInt();

            if (clientId == m_id) {
                qDebug() << "PC" << m_id << "received DHCP offer:" << offeredIP << "Assigning IP.";
                m_ipAddress->setIp(offeredIP);
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
    return m_ipAddress->getIp();
}

void PC::setMetricsCollector(QSharedPointer<MetricsCollector> collector) {
    m_metricsCollector = collector;
}
