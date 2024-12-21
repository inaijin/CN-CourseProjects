#ifndef METRICSCOLLECTOR_H
#define METRICSCOLLECTOR_H

#include <QObject>
#include <QMap>
#include <QString>
#include <vector>

class MetricsCollector : public QObject
{
    Q_OBJECT

public:
    explicit MetricsCollector(QObject *parent = nullptr);
    ~MetricsCollector() override = default;

    void recordPacketSent();
    void recordPacketReceived(int hopCount, const std::vector<QString> &path);
    void recordPacketDropped();

    void printStatistics() const;

private:
    int m_sentPackets;
    int m_receivedPackets;
    int m_droppedPackets;
    int m_totalHops;
    QMap<QString, int> m_routerUsage;
};

#endif // METRICSCOLLECTOR_H
