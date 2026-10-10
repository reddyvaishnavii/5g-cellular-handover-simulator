#ifndef HANDOVER_MANAGER_H
#define HANDOVER_MANAGER_H

#include <limits>
#include <vector>

#include "Cell.h"
#include "UserEquipment.h"

// Why the manager chose what it chose. Handy for logs, CSV export and tests.
enum class DecisionReason {
    StayBestAvailable,     // serving cell is fine, no neighbour is better by enough
    WaitingTimeToTrigger,  // a neighbour is better, but not for long enough yet
    SignalHandover,        // neighbour stayed better than serving + offset for the full TTT
    EmergencyFailover,     // serving cell is down, moved to the best reachable cell
    NoServiceAvailable     // serving cell is down and nothing can take the UE
};

struct HandoverDecision {
    Cell* target;          // cell the UE should be on after this decision
    DecisionReason reason;

    bool isHandover() const {
        return reason == DecisionReason::SignalHandover ||
               reason == DecisionReason::EmergencyFailover;
    }
};

class HandoverManager {
public:
    HandoverManager(
        double signalThresholdDb,       // required signal advantage (A3-style offset)
        double maxCandidateUtilization,
        double maxLatencyMs,
        double maxPacketLossRate,
        double timeToTriggerSeconds = 0.0,   // 0 = hand over immediately
        double minSignalDbm =
            -std::numeric_limits<double>::infinity()  // ignore cells weaker than this
    );

    // Look at every cell (serving cell may be in the list, it is skipped)
    // and decide where the UE should be. Stateful because of time-to-trigger,
    // so keep one HandoverManager per UE.
    HandoverDecision decide(
        Cell& serving,
        const std::vector<Cell*>& allCells,
        const UserEquipment& ue,
        double nowSeconds
    );

private:
    bool isHealthy(const Cell& cell) const;

    Cell* bestBySignal(
        const std::vector<Cell*>& cells,
        const UserEquipment& ue,
        const Cell* exclude,
        bool requireHealthy
    ) const;

    void resetTrigger();

    double signalThresholdDb;
    double maxCandidateUtilization;
    double maxLatencyMs;
    double maxPacketLossRate;
    double timeToTriggerSeconds;
    double minSignalDbm;

    // time-to-trigger state
    Cell* triggerTarget = nullptr;
    double triggerStartSeconds = 0.0;
};

#endif