#ifndef IDASSIGNMENT_H
#define IDASSIGNMENT_H

#include <QMap>

struct AsIdRange {
    int asId;
    int routerStartId;
    int routerEndId;

    int pcStartId;
    int pcEndId;
};

class IdAssignment {
public:
    void addAsIdRange(int asId, int routerStart, int routerEnd, int pcStart, int pcEnd) {
        AsIdRange range {asId, routerStart, routerEnd, pcStart, pcEnd};
        m_asIdRanges.insert(asId, range);
    }

    bool getAsIdRange(int asId, AsIdRange &outRange) const {
        if (m_asIdRanges.contains(asId)) {
            outRange = m_asIdRanges.value(asId);
            return true;
        }
        return false;
    }

private:
    QMap<int, AsIdRange> m_asIdRanges;
};

#endif // IDASSIGNMENT_H
