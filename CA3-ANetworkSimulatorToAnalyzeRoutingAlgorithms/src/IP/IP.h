#ifndef IP_H
#define IP_H

#include "Globals.h"

#include <QObject>
#include <QSharedPointer>
#include <QString>
#include <QTextStream>
#include <limits>
#include <cmath>

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
    virtual uint64_t toValue() const = 0;
    virtual void fromString(const QString &ipString) = 0;
    virtual void fromValue(uint64_t ipValue) = 0;

protected:
    uint64_t m_ipValue;
};

/**
 * Specialization of IP class for IPv4.
 */
template <>
class IP<UT::IPVersion::IPv4> : public AbstractIP
{
public:
    explicit IP(QObject *parent = nullptr);
    explicit IP(const QString &ipString, QObject *parent = nullptr);
    explicit IP(uint64_t ipValue, QObject *parent = nullptr);
    ~IP() override;

    QString toString() const override;
    uint64_t toValue() const override;
    void fromString(const QString &ipString) override;
    void fromValue(uint64_t ipValue) override;

    bool operator==(const IP<UT::IPVersion::IPv4> &other) const
    {
        return toValue() == other.toValue();
    }

private:
    QString toStringImpl() const;
    void fromStringImpl(const QString &ipString);
};

/**
 * Specialization of IP class for IPv6.
 */
template <>
class IP<UT::IPVersion::IPv6> : public AbstractIP
{
public:
    explicit IP(QObject *parent = nullptr);
    explicit IP(const QString &ipString, QObject *parent = nullptr);
    explicit IP(uint64_t ipValue, QObject *parent = nullptr);
    ~IP() override;

    QString toString() const override;
    uint64_t toValue() const override;
    void fromString(const QString &ipString) override;
    void fromValue(uint64_t ipValue) override;

    bool operator==(const IP<UT::IPVersion::IPv6> &other) const
    {
        return toValue() == other.toValue();
    }

private:
    QString toStringImpl() const;
    void fromStringImpl(const QString &ipString);
};

#endif // IP_H
