#include <cassert>
#include <iostream>

#include "Cell.h"
#include "UserEquipment.h"
#include "HandoverManager.h"

int main() {

    UserEquipment ue(
        "UE-001",
        110.0,
        0.0,
        0.0,
        0.0
    );

    HandoverManager manager(
        3.0,   // minimum signal advantage
        0.80,  // maximum utilization
        30.0,  // maximum latency
        0.05   // maximum packet loss
    );

    // --------------------------------------------------
    // Test 1: Healthy candidate -> handover
    // --------------------------------------------------

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

    Cell healthyCellB(
        "Cell-B",
        150.0,
        0.0,
        100.0,
        -50.0,
        20.0,
        0.02,
        100
    );

    Cell& selected1 =
        manager.selectBestCell(
            cellA,
            healthyCellB,
            ue
        );

    assert(selected1.getId() == "Cell-B");

    // --------------------------------------------------
    // Test 2: Overloaded candidate -> reject
    // --------------------------------------------------

    Cell overloadedCell(
        "Overloaded",
        150.0,
        0.0,
        100.0,
        -50.0,
        20.0,
        0.02,
        100
    );

    for (int i = 0; i < 90; i++) {
        overloadedCell.connectUser();
    }

    Cell& selected2 =
        manager.selectBestCell(
            cellA,
            overloadedCell,
            ue
        );

    assert(selected2.getId() == "Cell-A");

    // --------------------------------------------------
    // Test 3: High latency -> reject
    // --------------------------------------------------

    Cell highLatencyCell(
        "High-Latency",
        150.0,
        0.0,
        100.0,
        -50.0,
        50.0,
        0.02,
        100
    );

    Cell& selected3 =
        manager.selectBestCell(
            cellA,
            highLatencyCell,
            ue
        );

    assert(selected3.getId() == "Cell-A");

    // --------------------------------------------------
    // Test 4: High packet loss -> reject
    // --------------------------------------------------

    Cell highLossCell(
        "High-Loss",
        150.0,
        0.0,
        100.0,
        -50.0,
        20.0,
        0.10,
        100
    );

    Cell& selected4 =
        manager.selectBestCell(
            cellA,
            highLossCell,
            ue
        );

    assert(selected4.getId() == "Cell-A");

    // --------------------------------------------------
    // Test 5: Unavailable candidate -> reject
    // --------------------------------------------------

    Cell unavailableCell(
        "Unavailable",
        150.0,
        0.0,
        100.0,
        -50.0,
        20.0,
        0.02,
        100
    );

    unavailableCell.setAvailable(false);

    Cell& selected5 =
        manager.selectBestCell(
            cellA,
            unavailableCell,
            ue
        );

    assert(selected5.getId() == "Cell-A");

    // --------------------------------------------------
    // Test 6: Failed serving cell -> failover
    // --------------------------------------------------

    Cell failedServingCell(
        "Failed-Serving",
        0.0,
        0.0,
        100.0,
        -50.0,
        15.0,
        0.01,
        100
    );

    failedServingCell.setAvailable(false);

    Cell& selected6 =
        manager.selectBestCell(
            failedServingCell,
            healthyCellB,
            ue
        );

    assert(selected6.getId() == "Cell-B");

    std::cout
        << "All handover tests passed."
        << std::endl;

    return 0;
}