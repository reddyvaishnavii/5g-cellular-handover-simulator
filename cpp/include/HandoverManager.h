#ifndef HANDOVER_MANAGER_H
#define HANDOVER_MANAGER_H

#include "Cell.h"
#include "UserEquipment.h"

class HandoverManager {
public:
    HandoverManager(
        double signalThresholdDb,
        double maxCandidateUtilization,
        double maxLatencyMs,
        double maxPacketLossRate
    );

    Cell& selectBestCell(
        Cell& currentCell,
        Cell& candidateCell,
        const UserEquipment& ue
    ) const;

private:
    double signalThresholdDb;
    double maxCandidateUtilization;
    double maxLatencyMs;
    double maxPacketLossRate;
};

#endif