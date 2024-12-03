#ifndef MACADDRESS_H
#define MACADDRESS_H

#include <QString>

class MACAddress {
public:
    MACAddress();
    explicit MACAddress(const QString &address);

    QString toString() const;
    static bool isValid(const QString &address);

private:
    QString macAddress;
};

#endif // MACADDRESS_H
