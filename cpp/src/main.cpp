#include <fstream>
#include <iostream>

#include "Cell.h"
#include "UserEquipment.h"
#include "HandoverManager.h"

void printCellStatus(
    const Cell& cell,
    const UserEquipment& ue
) {
    std::cout
        << cell.getId()
        << " | distance = "
        << cell.distanceTo(ue.getX(), ue.getY())
        << " | signal = "
        << cell.calculateSignalDbm(ue.getX(), ue.getY())
        << " dBm"
        << " | latency = "
        << cell.getLatencyMs()
        << " ms"
        << " | packet loss = "
        << cell.getPacketLossRate() * 100
        << "%"
        << " | utilization = "
        << cell.getUtilization() * 100
        << "%"
        << std::endl;
}

int main() {

        // -------------------------
    // Open CSV output file
    // -------------------------
    std::ofstream csvFile("../data/simulation_results.csv");

    if (!csvFile.is_open()) {
        std::cerr
            << "Error: could not open CSV output file."
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
        << "cell_b_packet_loss"
        << "\n";

    // -------------------------
    // Create Cell A
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

    // -------------------------
    // Create Cell B
    // -------------------------
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

    // Simulate Cell B being heavily congested
    for (int i = 0; i < 50; i++) {
        cellB.connectUser();
    }

    std::cout
        << "Cell-B utilization: "
        << cellB.getUtilization() * 100
        << "%"
        << std::endl;

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

    // -------------------------
    // UE initially connected
    // to Cell A
    // -------------------------
    Cell* servingCell = &cellA;
    servingCell->connectUser();

    // -------------------------
    // Create handover manager
    //
    // 3.0  = minimum signal advantage
    // 0.80 = maximum candidate utilization
    // 30.0 = maximum latency
    // 0.05 = maximum packet loss (5%)
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
    for (int time = 0; time <= 30; time += 5) {

        std::cout
            << "\n=== Time: "
            << time
            << " seconds ==="
            << std::endl;

            // Simulate Cell A failure at 15 seconds.
    if (time == 15) {
        cellA.setAvailable(false);

        std::cout
            << "FAILURE: Cell-A is now unavailable"
            << std::endl;
    }

        // Simulate Cell A recovery at 25 seconds.
    if (time == 25) {
        cellA.setAvailable(true);

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

        // Show complete network status
        // for both cells.
        printCellStatus(cellA, ue);
        printCellStatus(cellB, ue);

        // Determine which cell is the candidate.
        Cell* candidateCell;

        if (servingCell == &cellA) {
            candidateCell = &cellB;
        } else {
            candidateCell = &cellA;
        }


        // Ask HandoverManager whether
        // the candidate should serve the UE.
        Cell& selectedCell =
            handoverManager.selectBestCell(
                *servingCell,
                *candidateCell,
                ue
            );

        // Check whether a handover happened.
        if (selectedCell.getId() != servingCell->getId()) {

            std::cout
                << "HANDOVER: "
                << servingCell->getId()
                << " -> "
                << selectedCell.getId()
                << std::endl;

            // Update connection counts.
            servingCell->disconnectUser();
            selectedCell.connectUser();

            // Update serving cell.
            servingCell = &selectedCell;

        } else {

            std::cout
                << "STAY: "
                << servingCell->getId()
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
            << cellB.getPacketLossRate()
            << "\n";

        // Move UE forward by 5 seconds.
        ue.move(5.0);
    }
        csvFile.close();
    return 0;
}