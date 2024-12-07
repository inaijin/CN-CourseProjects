#ifndef IPHEADER_H
#define IPHEADER_H

#include <QObject>
#include <QByteArray>
#include <QString>
#include <QSharedPointer>

// Abstract Base Header Class
class AbstractIPHeader : public QObject
{
    Q_OBJECT

public:
    explicit AbstractIPHeader(QObject *parent = nullptr);
    virtual ~AbstractIPHeader();

    virtual QByteArray toBytes() const = 0;
    virtual void fromBytes(const QByteArray &data) = 0;
};

class IPv4Header : public AbstractIPHeader
{
    Q_OBJECT

public:
    explicit IPv4Header(QObject *parent = nullptr);
    ~IPv4Header() override;

    QByteArray toBytes() const override;
    void fromBytes(const QByteArray &data) override;

    void setSourceAddress(const QString &address);
    void setDestinationAddress(const QString &address);
    QString getSourceAddress() const;
    QString getDestinationAddress() const;

private:
    uint8_t m_versionHeaderLength;
    uint8_t m_typeOfService;
    uint16_t m_totalLength;
    uint16_t m_identification;
    uint16_t m_flagsFragmentOffset;
    uint8_t m_ttl;
    uint8_t m_protocol;
    uint16_t m_headerChecksum;
    QString m_sourceAddress;
    QString m_destinationAddress;
};

class IPv6Header : public AbstractIPHeader
{
    Q_OBJECT

public:
    explicit IPv6Header(QObject *parent = nullptr);
    ~IPv6Header() override;

    QByteArray toBytes() const override;
    void fromBytes(const QByteArray &data) override;

    void setSourceAddress(const QString &address);
    void setDestinationAddress(const QString &address);
    QString getSourceAddress() const;
    QString getDestinationAddress() const;

private:
    uint32_t m_versionTrafficClassFlowLabel;
    uint16_t m_payloadLength;
    uint8_t m_nextHeader;
    uint8_t m_hopLimit;
    QString m_sourceAddress;
    QString m_destinationAddress;
};

#endif // IPHEADER_H
