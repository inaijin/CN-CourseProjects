#ifndef METRICSCOLLECTOR_H
#define METRICSCOLLECTOR_H

#include <QMap>
#include <QMutex>
#include <QString>
#include <QObject>

class MetricsCollector : public QObject
{
    Q_OBJECT

public:
    explicit MetricsCollector(QObject *parent = nullptr);
    ~MetricsCollector() override = default;

    void recordPacketSent();
    void recordPacketReceived(const QVector<QString> &path);
    void recordPacketDropped();

    void recordRouterUsage(const QString &routerIP);
    void recordHopCount(int hopCount);
    void recordWaitCycle(int waitCycle);

    void printStatistics() const;
    void increamentHops();

private:
    mutable QMutex m_mutex;

    int m_sentPackets;
    int m_receivedPackets;
    int m_droppedPackets;
    int m_totalHops;
    QVector<int> m_waitCyclesBuffer;
    QMap<QString, int> m_routerUsage;
};

#endif // METRICSCOLLECTOR_H
