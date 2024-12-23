#ifndef METRICSCOLLECTOR_H
#define METRICSCOLLECTOR_H

#include <QObject>
#include <QMap>
#include <QString>

class MetricsCollector : public QObject
{
    Q_OBJECT

public:
    explicit MetricsCollector(QObject *parent = nullptr);
    ~MetricsCollector() override = default;

    void recordPacketSent();
    void recordPacketReceived(int hopCount,  const QVector<QString> &path);
    void recordPacketDropped();

    void recordRouterUsage(const QString &routerIP);
    void recordHopCount(int hopCount);

    void printStatistics() const;

private:
    int m_sentPackets;
    int m_receivedPackets;
    int m_droppedPackets;
    int m_totalHops;
    QMap<QString, int> m_routerUsage;
};

#endif // METRICSCOLLECTOR_H
