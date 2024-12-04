#ifndef DATALINKHEADER_H
#define DATALINKHEADER_H

#include "../MACAddress/MACAddress.h"

class DataLinkHeader
{
public:
    explicit DataLinkHeader(const MACAddress &sourceMAC = MACAddress(),
                            const MACAddress &destinationMAC = MACAddress(),
                            const QString &frameType = "0x0800",
                            const QString &errorDetectionCode = "0000");

    void setSourceMAC(const MACAddress &sourceMAC);
    MACAddress getSourceMAC() const;

    void setDestinationMAC(const MACAddress &destinationMAC);
    MACAddress getDestinationMAC() const;

    void setFrameType(const QString &frameType);
    QString getFrameType() const;

    void setErrorDetectionCode(const QString &errorDetectionCode);
    QString getErrorDetectionCode() const;

    QString toString() const;

private:
    MACAddress m_sourceMAC;          // Source MAC Address
    MACAddress m_destinationMAC;     // Destination MAC Address
    QString m_frameType;             // Frame Type
    QString m_errorDetectionCode;    // Error Detection Code
};

#endif // DATALINKHEADER_H
