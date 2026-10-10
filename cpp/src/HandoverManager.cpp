#include "HandoverManager.h"

HandoverManager::HandoverManager(
    double signalThresholdDb,
    double maxCandidateUtilization,
    double maxLatencyMs,
    double maxPacketLossRate,
    double timeToTriggerSeconds,
    double minSignalDbm
)
    : signalThresholdDb(signalThresholdDb),
      maxCandidateUtilization(maxCandidateUtilization),
      maxLatencyMs(maxLatencyMs),
      maxPacketLossRate(maxPacketLossRate),
      timeToTriggerSeconds(timeToTriggerSeconds),
      minSignalDbm(minSignalDbm) {
}

bool HandoverManager::isHealthy(const Cell& cell) const {
    return cell.isAvailable()
        && cell.getUtilization() < maxCandidateUtilization
        && cell.getLatencyMs() <= maxLatencyMs
        && cell.getPacketLossRate() <= maxPacketLossRate;
}

Cell* HandoverManager::bestBySignal(
    const std::vector<Cell*>& cells,
    const UserEquipment& ue,
    const Cell* exclude,
    bool requireHealthy
) const {
    Cell* best = nullptr;
    double bestSignal = -std::numeric_limits<double>::infinity();

    for (Cell* cell : cells) {
        if (cell == nullptr || cell == exclude) continue;
        if (!cell->isAvailable()) continue;
        if (requireHealthy && !isHealthy(*cell)) continue;

        double signal = cell->calculateSignalDbm(ue.getX(), ue.getY());
        if (signal < minSignalDbm) continue;

        if (best == nullptr || signal > bestSignal) {
            best = cell;
            bestSignal = signal;
        }
    }
    return best;
}

void HandoverManager::resetTrigger() {
    triggerTarget = nullptr;
    triggerStartSeconds = 0.0;
}

HandoverDecision HandoverManager::decide(
    Cell& serving,
    const std::vector<Cell*>& allCells,
    const UserEquipment& ue,
    double nowSeconds
) {
    // 1) Serving cell is down -> emergency failover.
    //    Prefer a healthy cell, but a degraded cell beats a dead one.
    if (!serving.isAvailable()) {
        resetTrigger();

        Cell* target = bestBySignal(allCells, ue, &serving, true);
        if (target == nullptr) {
            target = bestBySignal(allCells, ue, &serving, false);
        }
        if (target != nullptr) {
            return {target, DecisionReason::EmergencyFailover};
        }
        return {&serving, DecisionReason::NoServiceAvailable};
    }

    // 2) Normal case: best healthy neighbour must beat serving by the offset.
    Cell* best = bestBySignal(allCells, ue, &serving, true);

    double servingSignal = serving.calculateSignalDbm(ue.getX(), ue.getY());
    bool hasAdvantage =
        best != nullptr &&
        best->calculateSignalDbm(ue.getX(), ue.getY()) >
            servingSignal + signalThresholdDb;

    if (!hasAdvantage) {
        resetTrigger();   // condition stopped holding -> restart the timer
        return {&serving, DecisionReason::StayBestAvailable};
    }

    // 3) Time-to-trigger: advantage must hold continuously for TTT seconds.
    if (timeToTriggerSeconds <= 0.0) {
        resetTrigger();
        return {best, DecisionReason::SignalHandover};
    }

    if (triggerTarget != best) {
        triggerTarget = best;
        triggerStartSeconds = nowSeconds;
    }

    if (nowSeconds - triggerStartSeconds >= timeToTriggerSeconds) {
        resetTrigger();
        return {best, DecisionReason::SignalHandover};
    }

    return {&serving, DecisionReason::WaitingTimeToTrigger};
}