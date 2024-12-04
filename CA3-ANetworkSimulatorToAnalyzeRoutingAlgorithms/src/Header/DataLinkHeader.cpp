#include "DataLinkHeader.h"

DataLinkHeader::DataLinkHeader(const MACAddress &sourceMAC,
                               const MACAddress &destinationMAC,
                               const QString &frameType,
                               const QString &errorDetectionCode)
    : m_sourceMAC(sourceMAC),
    m_destinationMAC(destinationMAC),
    m_frameType(frameType),
    m_errorDetectionCode(errorDetectionCode)
{
}

void DataLinkHeader::setSourceMAC(const MACAddress &sourceMAC) {
    m_sourceMAC = sourceMAC;
}

MACAddress DataLinkHeader::getSourceMAC() const {
    return m_sourceMAC;
}

void DataLinkHeader::setDestinationMAC(const MACAddress &destinationMAC) {
    m_destinationMAC = destinationMAC;
}

MACAddress DataLinkHeader::getDestinationMAC() const {
    return m_destinationMAC;
}

void DataLinkHeader::setFrameType(const QString &frameType) {
    m_frameType = frameType;
}

QString DataLinkHeader::getFrameType() const {
    return m_frameType;
}

void DataLinkHeader::setErrorDetectionCode(const QString &errorDetectionCode) {
    m_errorDetectionCode = errorDetectionCode;
}

QString DataLinkHeader::getErrorDetectionCode() const {
    return m_errorDetectionCode;
}

QString DataLinkHeader::toString() const {
    return QString("Source MAC: %1, Destination MAC: %2, Frame Type: %3, Error Detection Code: %4")
    .arg(m_sourceMAC.toString(), m_destinationMAC.toString(), m_frameType, m_errorDetectionCode);
}
