#ifndef HANDOVER_MANAGER_H
#define HANDOVER_MANAGER_H

#include "Cell.h"
#include "UserEquipment.h"

class HandoverManager {
public:
    HandoverManager(double signalThresholdDb);

    const Cell& selectBestCell(
        const Cell& currentCell,
        const Cell& candidateCell,
        const UserEquipment& ue
    ) const;

private:
    double signalThresholdDb;
};

#endif