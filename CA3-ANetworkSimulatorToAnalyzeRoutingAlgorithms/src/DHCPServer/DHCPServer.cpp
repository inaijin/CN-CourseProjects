#include <QDebug>
#include <QDateTime>

#include "../IP/IP.h"
#include "DHCPServer.h"
#include "../IP/IPHeader.h"
#include "../Network/Router.h"

DHCPServer::DHCPServer(int asId, const QSharedPointer<Router> &router, QObject *parent)
    : QObject(parent),
    m_asId(asId),
    m_router(router),
    m_nextAvailableId(1),
    m_currentTime(0)
{
    QString logFileName;
    if (m_asId == 1) {
        m_ipPrefix = "192.168.100.";
        logFileName = "D:/QTProjects/CN-CA3/CN-CA3/logs/dhcp_server_5.log";
    } else if (m_asId == 2) {
        m_ipPrefix = "192.168.200.";
        logFileName = "D:/QTProjects/CN-CA3/CN-CA3/logs/dhcp_server_23.log";
    } else {
        qWarning() << "Unsupported AS ID:" << m_asId;
        logFileName = "dhcp_server_unknown.log";
    }

    m_logFile.setFileName(logFileName);
    if (!m_logFile.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
        qWarning() << "Unable to open log file:" << logFileName;
    } else {
        m_logStream.setDevice(&m_logFile);
    }

    qDebug() << "DHCP Server initialized for AS ID:" << m_asId << "on Router ID:" << router->getId();
    writeLog(QString("DHCP Server initialized for AS ID: %1 on Router ID: %2")
               .arg(m_asId).arg(router->getId()));
}

DHCPServer::~DHCPServer()
{
    if (m_logFile.isOpen()) {
        m_logFile.close();
    }
}

void DHCPServer::receivePacket(const PacketPtr_t &packet)
{
    if (!packet || packet->getType() != PacketType::Control) {
        qWarning() << "DHCP Server received invalid packet.";
        writeLog("DHCP Server received invalid packet.");
        return;
    }

    QString payload = packet->getPayload();
    qDebug() << "DHCP Server processing packet with payload:" << payload;
    writeLog(QString("DHCP Server processing packet with payload: %1").arg(payload));

    if (payload.contains("DHCP_REQUEST")) {
        assignIP(packet);
    }
}

void DHCPServer::assignIP(const PacketPtr_t &packet)
{
    QStringList parts = packet->getPayload().split(":");
    if (parts.size() < 2) {
        qWarning() << "Malformed DHCP_REQUEST packet.";
        writeLog("Malformed DHCP_REQUEST packet.");
        return;
    }

    int clientId = parts[1].toInt();

    if (m_asId == 1) {
        if (!((clientId >= 1 && clientId <= 16) || (clientId >= 24 && clientId <= 31))) {
            QString msg = QString("Client %1 not in our AS (1)").arg(clientId);
            qDebug() << msg;
            writeLog(msg);
            return;
        }
    } else if (m_asId == 2) {
        if (!((clientId >= 17 && clientId <= 23) || (clientId >= 32 && clientId <= 38))) {
            QString msg = QString("Client %1 not in our AS (2)").arg(clientId);
            qDebug() << msg;
            writeLog(msg);
            return;
        }
    } else {
        QString msg = QString("Unsupported AS ID: %1").arg(m_asId);
        qWarning() << msg;
        writeLog(msg);
        return;
    }

    QString assigningMsg = QString("Assigning IP to client %1 in AS %2").arg(clientId).arg(m_asId);
    qDebug() << assigningMsg;
    writeLog(assigningMsg);

    for (const auto &lease : m_leases) {
        if (lease.clientId == clientId) {
            QString msg = QString("Client %1 already has an IP: %2. Re-sending offer.")
            .arg(clientId)
              .arg(lease.ipAddress);
            qDebug() << msg;
            writeLog(msg);
            sendOffer(lease);
            return;
        }
    }

    QString ipAddress = QString("%1%2").arg(m_ipPrefix).arg(clientId);

    if (packet->isIPv6()) {
        QString assignedIpAddress = ipAddress;
        qDebug() << "Packet indicates IPv6. Converting assigned IPv4 address to IPv6.";
        writeLog("Packet indicates IPv6. Converting assigned IPv4 address to IPv6.");

        QSharedPointer<IPv4Header> ipv4Header = QSharedPointer<IPv4Header>::create();
        ipv4Header->setSourceAddress(ipAddress);
        ipv4Header->setDestinationAddress("::");

        IP ip{ QSharedPointer<AbstractIPHeader>(ipv4Header), nullptr };

        if (!ip.convertToIPv6()) {
            QString msg = QString("Failed to convert IPv4 address %1 to IPv6 for client %2")
            .arg(ipAddress).arg(clientId);
            qWarning() << msg;
            writeLog(msg);
            return;
        }

        assignedIpAddress = ip.getIp();
        QString convertedMsg = QString("Converted IPv6 Address: %1").arg(assignedIpAddress);
        qDebug() << convertedMsg;
        writeLog(convertedMsg);

        for (const auto &lease : m_leases) {
            if (lease.ipAddress == assignedIpAddress) {
                QString msg = QString("IPv6 address %1 is already assigned. Cannot assign to client %2")
                .arg(assignedIpAddress).arg(clientId);
                qWarning() << msg;
                writeLog(msg);
                return;
            }
        }

        DHCPLease newLease;
        newLease.ipAddress = assignedIpAddress;
        newLease.clientId = clientId;

        m_leases.append(newLease);

        QString newLeaseMsg = QString("New IP assigned (IPv6): %1 for client %2")
                                .arg(newLease.ipAddress).arg(clientId);
        qDebug() << newLeaseMsg;
        writeLog(newLeaseMsg);

        sendOffer(newLease);
        return;
    }

    for (const auto &lease : m_leases) {
        if (lease.ipAddress == ipAddress) {
            QString msg = QString("IP address %1 is already assigned. Cannot assign to client %2")
            .arg(ipAddress).arg(clientId);
            qWarning() << msg;
            writeLog(msg);
            return;
        }
    }

    DHCPLease lease = { ipAddress, clientId, m_currentTime + LEASE_DURATION };
    m_leases.append(lease);

    QString newIpMsg = QString("New IP assigned: %1 for client %2").arg(ipAddress).arg(clientId);
    qDebug() << newIpMsg;
    writeLog(newIpMsg);

    sendOffer(lease);
}

void DHCPServer::sendOffer(const DHCPLease &lease)
{
    QString payload = QString("DHCP_OFFER:%1:%2").arg(lease.ipAddress).arg(lease.clientId);
    auto offerPacket = QSharedPointer<Packet>::create(PacketType::Control, payload);

    offerPacket->setTTL(10);

    if (m_router) {
        const auto &ports = m_router->getPorts();
        QString msg = QString("Sending DHCP offer from Router %1 to client %2 with IP %3")
                        .arg(m_router->getId())
                        .arg(lease.clientId)
                        .arg(lease.ipAddress);
        qDebug() << msg;
        writeLog(msg);

        QString portCountMsg = QString("Number of ports on Router %1: %2")
                                 .arg(m_router->getId())
                                 .arg(ports.size());
        qDebug() << portCountMsg;
        writeLog(portCountMsg);

        for (const auto &port : ports) {
            if (port->isConnected()) {
                port->sendPacket(offerPacket);

                QString broadcastMsg = QString("DHCP Server on Router %1 broadcasted DHCP offer: %2 via Port %3")
                                         .arg(m_router->getId())
                                         .arg(payload)
                                         .arg(port->getPortNumber());
                qDebug() << broadcastMsg;
                writeLog(broadcastMsg);
            }
        }
    } else {
        qWarning() << "DHCP Server has no associated Router to broadcast offers.";
        writeLog("DHCP Server has no associated Router to broadcast offers.");
    }
}

void DHCPServer::tick(int currentTime)
{
    m_currentTime = currentTime;
    reclaimExpiredLeases();
}

void DHCPServer::reclaimExpiredLeases()
{
    for (int i = m_leases.size() - 1; i >= 0; --i) {
        if (m_leases[i].leaseExpirationTime <= m_currentTime) {
            QString msg = QString("Reclaiming expired IP: %1").arg(m_leases[i].ipAddress);
            qDebug() << msg;
            writeLog(msg);

            m_leases.removeAt(i);
        }
    }
}

void DHCPServer::writeLog(const QString &message)
{
    if (m_logFile.isOpen()) {
        m_logStream << QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss")
                    << " - " << message << "\n";
        m_logStream.flush();
    }
}
