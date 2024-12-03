#ifndef IP_H
#define IP_H

#include "Globals.h"

#include <QObject>
#include <QSharedPointer>
#include <QString>
#include <QTextStream>
#include <QHostAddress>
#include <QByteArray>
#include <QVariant>

// Template Definitions
template <UT::IPVersion version>
class IP;

typedef IP<UT::IPVersion::IPv4> IPv4_t;
typedef IP<UT::IPVersion::IPv6> IPv6_t;
typedef QSharedPointer<IPv4_t> IPv4Ptr_t;
typedef QSharedPointer<IPv6_t> IPv6Ptr_t;

// Abstract Base Class
class AbstractIP : public QObject
{
    Q_OBJECT

public:
    explicit AbstractIP(QObject *parent = nullptr);

    virtual QString toString() const = 0;
    virtual void fromString(const QString &ipString) = 0;
};

template <>
class IP<UT::IPVersion::IPv4> : public AbstractIP
{
public:
    explicit IP(QObject *parent = nullptr);
    explicit IP(const QString &ipString, QObject *parent = nullptr);
    explicit IP(uint32_t ipValue, QObject *parent = nullptr);
    ~IP() override;

    QString toString() const override;
    uint32_t toValue() const;
    void fromString(const QString &ipString) override;
    void fromValue(uint32_t ipValue);

    bool operator==(const IP<UT::IPVersion::IPv4> &other) const
    {
        return m_ipValue == other.m_ipValue;
    }

private:
    QString toStringImpl() const;
    void fromStringImpl(const QString &ipString);

    // IPv4 Header Fields
    uint8_t m_versionHeaderLength;
    uint8_t m_typeOfService;
    uint16_t m_totalLength;
    uint16_t m_identification;
    uint16_t m_flagsFragmentOffset;
    uint8_t m_ttl;
    uint8_t m_protocol;
    uint16_t m_headerChecksum;
    IPv4Ptr_t m_sourceIp;
    IPv4Ptr_t m_destIp;

    uint32_t m_ipValue;
};

template <>
class IP<UT::IPVersion::IPv6> : public AbstractIP
{
public:
    explicit IP(QObject *parent = nullptr);
    explicit IP(const QString &ipString, QObject *parent = nullptr);
    explicit IP(const Q_IPV6ADDR &ipValue, QObject *parent = nullptr);
    ~IP() override;

    QString toString() const override;
    Q_IPV6ADDR toValue() const;
    void fromString(const QString &ipString) override;
    void fromValue(const Q_IPV6ADDR &ipValue);

    bool operator==(const IP<UT::IPVersion::IPv6> &other) const;

private:
    QString toStringImpl() const;
    void fromStringImpl(const QString &ipString);

    // IPv6 Header Fields
    uint32_t m_versionTrafficClassFlowLabel;
    uint16_t m_payloadLength;
    uint8_t m_nextHeader;
    uint8_t m_hopLimit;
    IPv6Ptr_t m_sourceIp;
    IPv6Ptr_t m_destIp;

    Q_IPV6ADDR m_ipValue;
};

#endif // IP_H
