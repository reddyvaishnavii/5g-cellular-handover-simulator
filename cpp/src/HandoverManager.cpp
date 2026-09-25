#include "HandoverManager.h"

HandoverManager::HandoverManager(
    double signalThresholdDb,
    double maxCandidateUtilization,
    double maxLatencyMs,
    double maxPacketLossRate
)
    : signalThresholdDb(signalThresholdDb),
      maxCandidateUtilization(maxCandidateUtilization),
      maxLatencyMs(maxLatencyMs),
      maxPacketLossRate(maxPacketLossRate) {
}

Cell& HandoverManager::selectBestCell(
    Cell& currentCell,
    Cell& candidateCell,
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

    double candidateUtilization =
        candidateCell.getUtilization();

    double candidateLatency =
        candidateCell.getLatencyMs();

    double candidatePacketLoss =
        candidateCell.getPacketLossRate();

    // Reject an unavailable candidate cell.
    if (!candidateCell.isAvailable()) {
        return currentCell;
    }

    // Reject an overloaded candidate cell.
    if (candidateUtilization >= maxCandidateUtilization) {
        return currentCell;
    }

    // Reject a candidate with excessive latency.
    if (candidateLatency > maxLatencyMs) {
        return currentCell;
    }

    // Reject a candidate with excessive packet loss.
    if (candidatePacketLoss > maxPacketLossRate) {
        return currentCell;
    }

    // Emergency failover:
    // If the serving cell has failed, move to the
    // available candidate without requiring a signal advantage.
    if (!currentCell.isAvailable()) {
        return candidateCell;
    }

    // Normal handover:
    // Require the candidate to have a meaningful
    // signal advantage before handing over.
    if (candidateSignal >
        currentSignal + signalThresholdDb) {

        return candidateCell;
    }

    return currentCell;
}