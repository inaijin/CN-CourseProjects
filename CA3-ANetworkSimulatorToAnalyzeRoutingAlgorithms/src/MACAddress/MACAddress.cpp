#include "MACAddress.h"
#include <QRegularExpression>

MACAddress::MACAddress() : macAddress("00:00:00:00:00:00") {}

MACAddress::MACAddress(const QString &address) {
    if (isValid(address)) {
        macAddress = address;
    } else {
        throw std::invalid_argument("Invalid MAC address format");
    }
}

QString MACAddress::toString() const {
    return macAddress;
}

bool MACAddress::isValid(const QString &address) {
    static const QRegularExpression regex("([0-9A-Fa-f]{2}:){5}[0-9A-Fa-f]{2}");
    return regex.match(address).hasMatch();
}
