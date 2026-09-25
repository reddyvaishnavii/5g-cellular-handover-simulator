#include "HandoverManager.h"

HandoverManager::HandoverManager(double signalThresholdDb)
    : signalThresholdDb(signalThresholdDb) {
}

const Cell& HandoverManager::selectBestCell(
    const Cell& currentCell,
    const Cell& candidateCell,
    const UserEquipment& ue
) const {

    double currentSignal =
        currentCell.calculateSignalDbm(
            ue.getX(),
            ue.getY()
        );

    double candidateSignal =
        candidateCell.calculateSignalDbm(
            ue.getX(),
            ue.getY()
        );

    if (candidateSignal >
        currentSignal + signalThresholdDb) {

        return candidateCell;
    }

    return currentCell;
}