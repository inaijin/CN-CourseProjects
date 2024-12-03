#ifndef MACADDRESSGENERATOR_H
#define MACADDRESSGENERATOR_H

#include "MACAddress.h"
#include <QSet>
#include <QString>

class MACAddressGenerator {
public:
    static MACAddress generate();

private:
    static QSet<QString> usedAddresses;
    static QString generateRandomAddress();
};

#endif // MACADDRESSGENERATOR_H
