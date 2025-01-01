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
    QMutexLocker locker(&m_mutex);
    m_sentPackets++;
}

void MetricsCollector::recordPacketReceived(const QVector<QString> &path) {
    QMutexLocker locker(&m_mutex);
    m_receivedPackets++;

    for (const auto &routerIP : path) {
        if (routerIP.startsWith("192.168.")) {
            m_routerUsage[routerIP]++;
        }
    }
}

void MetricsCollector::recordWaitCycle(int waitCycle) {
    m_waitCyclesBuffer.append(waitCycle);
}

void MetricsCollector::increamentHops() { m_totalHops++; }

void MetricsCollector::recordPacketDropped() {
    QMutexLocker locker(&m_mutex);
    m_droppedPackets++;
}

void MetricsCollector::recordRouterUsage(const QString &routerIP) {
    QMutexLocker locker(&m_mutex);
    if (routerIP.startsWith("192.168.")) {
        m_routerUsage[routerIP]++;
    }
}

void MetricsCollector::recordHopCount(int hopCount) {
    QMutexLocker locker(&m_mutex);
    m_totalHops += hopCount;
}

void MetricsCollector::printStatistics() const {
    QMutexLocker locker(&m_mutex);

    qDebug() << "---- Simulation Metrics ----";
    qDebug() << "Total Packets Sent:" << m_sentPackets;
    qDebug() << "Total Packets Received:" << m_receivedPackets;
    qDebug() << "Total Packets Dropped:" << m_droppedPackets;

    double lossRate = (m_sentPackets > 0) ? 100 - (((double)m_receivedPackets / m_sentPackets) * 100.0) : 0.0;
    qDebug() << "Packet Loss Rate:" << lossRate << "%";

    double averageHopCount = (m_receivedPackets > 0) ? ((double)m_totalHops / m_receivedPackets) : 0.0;
    qDebug() << "Total Hop " << m_totalHops << " // Average Hop Count:" << averageHopCount;

    if (!m_waitCyclesBuffer.isEmpty()) {
        qDebug() << "Processing All Wait Cycles...";
        int totalWaitCycles = 0;
        int minWaitCycles = std::numeric_limits<int>::max();
        int maxWaitCycles = std::numeric_limits<int>::min();

        for (const int& waitCycle : m_waitCyclesBuffer) {

            totalWaitCycles += waitCycle;
            if (waitCycle < minWaitCycles) {
                minWaitCycles = waitCycle;
            }
            if (waitCycle > maxWaitCycles) {
                maxWaitCycles = waitCycle;
            }
        }

        double averageWaitCycles = (m_waitCyclesBuffer.size() > 0) ? static_cast<double>(totalWaitCycles) / m_waitCyclesBuffer.size() : 0.0;

        qDebug() << "Wait Cycles Statistics (All Data):";
        qDebug() << "Total Wait Cycles:" << totalWaitCycles;
        qDebug() << "Minimum Wait Cycles:" << minWaitCycles;
        qDebug() << "Maximum Wait Cycles:" << maxWaitCycles;
        qDebug() << "Average Wait Cycles:" << averageWaitCycles;
    } else {
        qDebug() << "No wait cycles data available.";
    }

    const int MIN_WAIT_CYCLES = 0;
    const int MAX_WAIT_CYCLES = 1000;

    QVector<int> filteredWaitCycles;
    int excludedCount = 0;
    for (const int& waitCycle : m_waitCyclesBuffer) {
        if (waitCycle >= MIN_WAIT_CYCLES && waitCycle <= MAX_WAIT_CYCLES) {
            filteredWaitCycles.append(waitCycle);
        } else {
            excludedCount++;
        }
    }

    if (!filteredWaitCycles.isEmpty()) {
        qDebug() << "Processing Filtered Wait Cycles (Excluding Outliers)...";
        int totalFilteredWaitCycles = 0;
        int minFilteredWaitCycles = std::numeric_limits<int>::max();
        int maxFilteredWaitCycles = std::numeric_limits<int>::min();

        for (const int& waitCycle : filteredWaitCycles) {

            totalFilteredWaitCycles += waitCycle;
            if (waitCycle < minFilteredWaitCycles) {
                minFilteredWaitCycles = waitCycle;
            }
            if (waitCycle > maxFilteredWaitCycles) {
                maxFilteredWaitCycles = waitCycle;
            }
        }

        double averageFilteredWaitCycles = (filteredWaitCycles.size() > 0) ? static_cast<double>(totalFilteredWaitCycles) / filteredWaitCycles.size() : 0.0;

        qDebug() << "Wait Cycles Statistics (Filtered Data):";
        qDebug() << "Total Wait Cycles:" << totalFilteredWaitCycles;
        qDebug() << "Minimum Wait Cycles:" << minFilteredWaitCycles;
        qDebug() << "Maximum Wait Cycles:" << maxFilteredWaitCycles;
        qDebug() << "Average Wait Cycles:" << averageFilteredWaitCycles;
        qDebug() << "Excluded Wait Cycles:" << excludedCount << "values outside the range [" << MIN_WAIT_CYCLES << ", " << MAX_WAIT_CYCLES << "].";
    } else {
        qDebug() << "No wait cycles data available within the acceptable range.";
    }

    qDebug() << "Router Usage:";
    if (m_routerUsage.isEmpty()) {
        qDebug() << "No router usage data available.";
    } else {
        for (auto it = m_routerUsage.constBegin(); it != m_routerUsage.constEnd(); ++it) {
            qDebug() << it.key() << ":" << it.value() << "packets";
        }
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
