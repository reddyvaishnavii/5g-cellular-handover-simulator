#include <iostream>

#include "Cell.h"
#include "UserEquipment.h"
#include "HandoverManager.h"

void printSignal(const Cell& cell, const UserEquipment& ue) {
    std::cout
        << cell.getId()
        << " | distance = "
        << cell.distanceTo(ue.getX(), ue.getY())
        << " | signal = "
        << cell.calculateSignalDbm(ue.getX(), ue.getY())
        << " dBm"
        << std::endl;
}

int main() {

    // -------------------------
    // Create Cell A
    // -------------------------
    Cell cellA(
        "Cell-A",
        0.0,
        0.0,
        100.0,
        -50.0,
        15.0,
        0.01,
        100
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
        20.0,
        0.02,
        100
    );

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
    // Create handover manager
    // 3 dB = minimum advantage
    // required for handover
    // -------------------------
    HandoverManager handoverManager(3.0);

    // UE initially connected to Cell A
    const Cell* servingCell = &cellA;

    // -------------------------
    // SIMULATION LOOP
    // -------------------------
    for (int time = 0; time <= 30; time += 5) {

        std::cout
            << "\n=== Time: "
            << time
            << " seconds ==="
            << std::endl;

        std::cout
            << "UE position: ("
            << ue.getX()
            << ", "
            << ue.getY()
            << ")"
            << std::endl;

        // Show signal from both cells
        printSignal(cellA, ue);
        printSignal(cellB, ue);

        // Determine which cell is the candidate
        const Cell* candidateCell;

        if (servingCell == &cellA) {
            candidateCell = &cellB;
        } else {
            candidateCell = &cellA;
        }

        // Ask HandoverManager which cell should serve the UE
        const Cell& selectedCell =
            handoverManager.selectBestCell(
                *servingCell,
                *candidateCell,
                ue
            );

        // Check whether a handover happened
        if (selectedCell.getId() != servingCell->getId()) {

            std::cout
                << "HANDOVER: "
                << servingCell->getId()
                << " -> "
                << selectedCell.getId()
                << std::endl;

            servingCell = &selectedCell;

        } else {

            std::cout
                << "STAY: "
                << servingCell->getId()
                << std::endl;
        }

        // Move UE forward by 5 seconds
        ue.move(5.0);
    }

    return 0;
}