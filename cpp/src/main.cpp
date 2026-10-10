#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "Cell.h"
#include "UserEquipment.h"
#include "HandoverManager.h"

namespace {

// Simulation settings (change these in one place).
const double kTimeStepSeconds     = 5.0;
const double kDurationSeconds     = 30.0;
const double kFailureTimeSeconds  = 15.0;   // Cell-A goes down
const double kRecoveryTimeSeconds = 25.0;   // Cell-A comes back

const char* reasonToString(DecisionReason reason) {
    switch (reason) {
        case DecisionReason::StayBestAvailable:    return "StayBestAvailable";
        case DecisionReason::WaitingTimeToTrigger: return "WaitingTimeToTrigger";
        case DecisionReason::SignalHandover:       return "SignalHandover";
        case DecisionReason::EmergencyFailover:    return "EmergencyFailover";
        case DecisionReason::NoServiceAvailable:   return "NoServiceAvailable";
    }
    return "Unknown";
}

void printCellStatus(const Cell& cell, const UserEquipment& ue) {
    std::cout
        << cell.getId()
        << " | distance = " << cell.distanceTo(ue.getX(), ue.getY())
        << " | signal = " << cell.calculateSignalDbm(ue.getX(), ue.getY())
        << " dBm"
        << " | latency = " << cell.getLatencyMs() << " ms"
        << " | packet loss = " << cell.getPacketLossRate() * 100 << "%"
        << " | utilization = " << cell.getUtilization() * 100 << "%"
        << " | " << (cell.isAvailable() ? "UP" : "DOWN")
        << std::endl;
}

}  // namespace

int main(int argc, char* argv[]) {

    // -------------------------
    // Open CSV output file
    // Usage: ./sim [output.csv]
    // Default path assumes you run from the build/ folder.
    // -------------------------
    const std::string csvPath =
        (argc > 1) ? argv[1] : "../data/simulation_results.csv";

    std::ofstream csvFile(csvPath);

    if (!csvFile.is_open()) {
        std::cerr
            << "Error: could not open CSV output file: "
            << csvPath
            << std::endl;
        return 1;
    }

    csvFile
        << "time,"
        << "serving_cell,"
        << "cell_a_signal,"
        << "cell_b_signal,"
        << "cell_a_utilization,"
        << "cell_b_utilization,"
        << "cell_a_latency,"
        << "cell_b_latency,"
        << "cell_a_packet_loss,"
        << "cell_b_packet_loss,"
        << "decision_reason"
        << "\n";

    // -------------------------
    // Create cells
    // -------------------------
    Cell cellA(
        "Cell-A",
        0.0,
        0.0,
        100.0,
        -50.0,
        15.0,      // latency: 15 ms
        0.01,      // packet loss: 1%
        100        // capacity
    );

    Cell cellB(
        "Cell-B",
        150.0,
        0.0,
        100.0,
        -50.0,
        20.0,      // latency: 20 ms
        0.02,      // packet loss: 2%
        100
    );

    // Simulate Cell B being moderately congested
    for (int i = 0; i < 50; i++) {
        cellB.connectUser();
    }

    std::cout
        << "Cell-B utilization: "
        << cellB.getUtilization() * 100
        << "%"
        << std::endl;

    // The manager looks at every cell in this list.
    // Adding Cell-C later is just: allCells.push_back(&cellC);
    std::vector<Cell*> allCells = { &cellA, &cellB };

    // -------------------------
    // Create mobile UE
    // -------------------------
    UserEquipment ue(
        "UE-001",
        10.0,
        0.0,
        5.0,
        0.0
    );

    // UE initially connected to Cell A
    Cell* servingCell = &cellA;
    servingCell->connectUser();

    // -------------------------
    // Create handover manager (one per UE, it keeps the TTT timer)
    //
    // 3.0  = minimum signal advantage (dB)
    // 0.80 = maximum candidate utilization
    // 30.0 = maximum latency (ms)
    // 0.05 = maximum packet loss (5%)
    // Optional extras:
    //   5th arg = time-to-trigger in seconds (0 = off)
    //   6th arg = minimum usable signal in dBm
    // -------------------------
    HandoverManager handoverManager(
        3.0,
        0.80,
        30.0,
        0.05
    );

    // -------------------------
    // SIMULATION LOOP
    // -------------------------
    bool failureDone  = false;
    bool recoveryDone = false;

    const int steps =
        static_cast<int>(kDurationSeconds / kTimeStepSeconds + 0.5);

    for (int step = 0; step <= steps; step++) {

        const double time = step * kTimeStepSeconds;

        std::cout
            << "\n=== Time: "
            << time
            << " seconds ==="
            << std::endl;

        // Simulate Cell A failure.
        if (!failureDone && time >= kFailureTimeSeconds) {
            cellA.setAvailable(false);
            failureDone = true;

            std::cout
                << "FAILURE: Cell-A is now unavailable"
                << std::endl;
        }

        // Simulate Cell A recovery.
        if (!recoveryDone && time >= kRecoveryTimeSeconds) {
            cellA.setAvailable(true);
            recoveryDone = true;

            std::cout
                << "RECOVERY: Cell-A is available again"
                << std::endl;
        }

        std::cout
            << "UE position: ("
            << ue.getX()
            << ", "
            << ue.getY()
            << ")"
            << std::endl;

        // Show complete network status for every cell.
        for (const Cell* cell : allCells) {
            printCellStatus(*cell, ue);
        }

        // Ask the HandoverManager where the UE should be.
        HandoverDecision decision =
            handoverManager.decide(
                *servingCell,
                allCells,
                ue,
                time
            );

        if (decision.isHandover()) {

            std::cout
                << "HANDOVER ("
                << reasonToString(decision.reason)
                << "): "
                << servingCell->getId()
                << " -> "
                << decision.target->getId()
                << std::endl;

            // Update connection counts.
            servingCell->disconnectUser();
            decision.target->connectUser();

            // Update serving cell.
            servingCell = decision.target;

        } else {

            std::cout
                << "STAY: "
                << servingCell->getId()
                << " ("
                << reasonToString(decision.reason)
                << ")"
                << std::endl;
        }

        // -------------------------
        // Write simulation data
        // -------------------------
        csvFile
            << time << ","
            << servingCell->getId() << ","
            << cellA.calculateSignalDbm(ue.getX(), ue.getY()) << ","
            << cellB.calculateSignalDbm(ue.getX(), ue.getY()) << ","
            << cellA.getUtilization() << ","
            << cellB.getUtilization() << ","
            << cellA.getLatencyMs() << ","
            << cellB.getLatencyMs() << ","
            << cellA.getPacketLossRate() << ","
            << cellB.getPacketLossRate() << ","
            << reasonToString(decision.reason)
            << "\n";

        // Move UE forward by one time step.
        ue.move(kTimeStepSeconds);
    }

    csvFile.close();
    return 0;
}