#include "MACAddressGenerator.h"
#include <QRandomGenerator>

QSet<QString> MACAddressGenerator::usedAddresses;

MACAddress MACAddressGenerator::generate() {
    QString newAddress;
    do {
        newAddress = generateRandomAddress();
    } while (usedAddresses.contains(newAddress));

    usedAddresses.insert(newAddress);
    return MACAddress(newAddress);
}

QString MACAddressGenerator::generateRandomAddress() {
    QString address;
    for (int i = 0; i < 6; ++i) {
        int byte = QRandomGenerator::global()->bounded(0, 256);
        address += QString::asprintf("%02X", byte);
        if (i < 5) address += ":";
    }
    return address;
}
