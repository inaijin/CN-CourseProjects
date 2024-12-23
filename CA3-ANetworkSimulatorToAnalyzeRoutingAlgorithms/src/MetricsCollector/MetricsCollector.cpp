#include "MetricsCollector.h"
#include <QDebug>

MetricsCollector::MetricsCollector(QObject *parent)
    : QObject(parent),
      m_sentPackets(0),
      m_receivedPackets(0),
      m_droppedPackets(0),
      m_totalHops(0)
{
}

void MetricsCollector::recordPacketSent() {
    m_sentPackets++;
}

void MetricsCollector::recordPacketReceived(int hopCount, const QVector<QString> &path) {
    qDebug() << "chera naraft !!!";
    m_receivedPackets++;
    m_totalHops += hopCount;

    for (const auto &routerIP : path) {
        if (routerIP.startsWith("192.168.")) {
            m_routerUsage[routerIP]++;
        }
    }
}

void MetricsCollector::recordPacketDropped() {
    m_droppedPackets++;
}

void MetricsCollector::recordRouterUsage(const QString &routerIP) {
    if (routerIP.startsWith("192.168.")) {
        m_routerUsage[routerIP]++;
    }
}

void MetricsCollector::recordHopCount(int hopCount) {
    m_totalHops += hopCount;
}

void MetricsCollector::printStatistics() const {
    qDebug() << "---- Simulation Metrics ----";
    qDebug() << "Total Packets Sent:" << m_sentPackets;
    qDebug() << "Total Packets Received:" << m_receivedPackets;
    qDebug() << "Total Packets Dropped:" << m_droppedPackets;

    double lossRate = (m_sentPackets > 0) ? ((double)m_droppedPackets / m_sentPackets) * 100.0 : 0.0;
    qDebug() << "Packet Loss Rate:" << lossRate << "%";

    double averageHopCount = (m_receivedPackets > 0) ? ((double)m_totalHops / m_receivedPackets) : 0.0;
    qDebug() << "Average Hop Count:" << averageHopCount;

    qDebug() << "Router Usage:";
    for (auto it = m_routerUsage.constBegin(); it != m_routerUsage.constEnd(); ++it) {
        qDebug() << it.key() << ":" << it.value() << "packets";
    }

    QString poorRouter;
    int maxUsage = 0;
    for (auto it = m_routerUsage.constBegin(); it != m_routerUsage.constEnd(); ++it) {
        if (it.value() > maxUsage) {
            maxUsage = it.value();
            poorRouter = it.key();
        }
    }

    if (!poorRouter.isEmpty()) {
        qDebug() << "Poor Router (Most Used):" << poorRouter << "with" << maxUsage << "packets.";
    } else {
        qDebug() << "No router usage data available.";
    }
}
