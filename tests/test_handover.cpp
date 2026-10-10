#include <iostream>
#include <string>
#include <vector>

#include "Cell.h"
#include "HandoverManager.h"
#include "UserEquipment.h"

// assert() is compiled out when NDEBUG is defined (Release builds), which
// would make every test pass without checking anything. This macro always runs.
static int failures = 0;
#define CHECK(cond)                                                       \
    do {                                                                  \
        if (!(cond)) {                                                    \
            std::cerr << "FAILED (line " << __LINE__ << "): " #cond       \
                      << std::endl;                                       \
            ++failures;                                                   \
        }                                                                 \
    } while (0)

static Cell makeCell(const std::string& id, double x,
                     double latencyMs = 20.0, double loss = 0.02) {
    return Cell(id, x, 0.0, 100.0, -50.0, latencyMs, loss, 100);
}

static UserEquipment ueAt(double x) {
    return UserEquipment("UE-001", x, 0.0, 0.0, 0.0);
}

static HandoverManager defaultManager() {
    return HandoverManager(3.0, 0.80, 30.0, 0.05);
}

static void congest(Cell& cell, int users) {
    for (int i = 0; i < users; i++) cell.connectUser();
}

// ---------------------------------------------------------------- basics

static void testHealthyCandidateTriggersHandover() {
    Cell a = makeCell("Cell-A", 0.0, 15.0, 0.01);
    Cell b = makeCell("Cell-B", 150.0);
    std::vector<Cell*> cells = {&a, &b};
    HandoverManager m = defaultManager();

    HandoverDecision d = m.decide(a, cells, ueAt(110.0), 0.0);
    CHECK(d.target == &b);
    CHECK(d.reason == DecisionReason::SignalHandover);
    CHECK(d.isHandover());
}

static void testRejectsBadCandidates() {
    UserEquipment ue = ueAt(110.0);

    Cell overloaded = makeCell("Overloaded", 150.0);
    congest(overloaded, 90);
    Cell slow = makeCell("Slow", 150.0, 50.0, 0.02);
    Cell lossy = makeCell("Lossy", 150.0, 20.0, 0.10);
    Cell down = makeCell("Down", 150.0);
    down.setAvailable(false);

    Cell* bad[] = {&overloaded, &slow, &lossy, &down};
    for (Cell* candidate : bad) {
        Cell a = makeCell("Cell-A", 0.0, 15.0, 0.01);
        std::vector<Cell*> cells = {&a, candidate};
        HandoverManager m = defaultManager();

        HandoverDecision d = m.decide(a, cells, ue, 0.0);
        if (d.target != &a) {
            std::cerr << "  candidate wrongly accepted: "
                      << candidate->getId() << std::endl;
        }
        CHECK(d.target == &a);
        CHECK(d.reason == DecisionReason::StayBestAvailable);
    }
}

static void testEqualSignalStaysPut() {
    Cell a = makeCell("Cell-A", 0.0);
    Cell b = makeCell("Cell-B", 150.0);
    std::vector<Cell*> cells = {&a, &b};
    HandoverManager m = defaultManager();

    // Exactly halfway: identical signal, no advantage -> no handover.
    HandoverDecision d = m.decide(a, cells, ueAt(75.0), 0.0);
    CHECK(d.target == &a);
    CHECK(!d.isHandover());
}

// ---------------------------------------------------------------- failover

static void testFailoverWhenServingCellDown() {
    Cell a = makeCell("Cell-A", 0.0);
    Cell b = makeCell("Cell-B", 150.0);
    std::vector<Cell*> cells = {&a, &b};
    a.setAvailable(false);
    HandoverManager m = defaultManager();

    HandoverDecision d = m.decide(a, cells, ueAt(10.0), 0.0);
    CHECK(d.target == &b);
    CHECK(d.reason == DecisionReason::EmergencyFailover);
}

static void testNoServiceWhenEverythingDown() {
    Cell a = makeCell("Cell-A", 0.0);
    Cell b = makeCell("Cell-B", 150.0);
    std::vector<Cell*> cells = {&a, &b};
    a.setAvailable(false);
    b.setAvailable(false);
    HandoverManager m = defaultManager();

    HandoverDecision d = m.decide(a, cells, ueAt(10.0), 0.0);
    CHECK(d.target == &a);
    CHECK(d.reason == DecisionReason::NoServiceAvailable);
    CHECK(!d.isHandover());
}

static void testFailoverAcceptsDegradedCellIfNothingHealthy() {
    Cell a = makeCell("Cell-A", 0.0);
    Cell b = makeCell("Cell-B", 150.0);
    congest(b, 90);   // 90% utilised -> not "healthy", but alive
    std::vector<Cell*> cells = {&a, &b};
    a.setAvailable(false);
    HandoverManager m = defaultManager();

    HandoverDecision d = m.decide(a, cells, ueAt(110.0), 0.0);
    CHECK(d.target == &b);
    CHECK(d.reason == DecisionReason::EmergencyFailover);
}

static void testFailoverPrefersHealthyOverStrongerDegraded() {
    Cell a = makeCell("Cell-A", 0.0);
    Cell strongButCongested = makeCell("Strong-Congested", 120.0);
    congest(strongButCongested, 90);
    Cell healthy = makeCell("Healthy", 150.0);
    std::vector<Cell*> cells = {&a, &strongButCongested, &healthy};
    a.setAvailable(false);
    HandoverManager m = defaultManager();

    HandoverDecision d = m.decide(a, cells, ueAt(110.0), 0.0);
    CHECK(d.target == &healthy);
}

// ---------------------------------------------------------------- multi-cell

static void testPicksStrongestEligibleOfSeveral() {
    Cell a = makeCell("Cell-A", 0.0);
    Cell closeButCongested = makeCell("Close-Congested", 120.0);
    congest(closeButCongested, 90);
    Cell good = makeCell("Good", 150.0);
    Cell far = makeCell("Far", 300.0);
    std::vector<Cell*> cells = {&a, &closeButCongested, &good, &far};
    HandoverManager m = defaultManager();

    HandoverDecision d = m.decide(a, cells, ueAt(110.0), 0.0);
    CHECK(d.target == &good);
}

// ---------------------------------------------------------------- tuning knobs

static void testTimeToTrigger() {
    Cell a = makeCell("Cell-A", 0.0);
    Cell b = makeCell("Cell-B", 150.0);
    std::vector<Cell*> cells = {&a, &b};
    HandoverManager m(3.0, 0.80, 30.0, 0.05, /*TTT=*/1.0);
    UserEquipment ue = ueAt(110.0);

    HandoverDecision d0 = m.decide(a, cells, ue, 0.0);
    CHECK(d0.reason == DecisionReason::WaitingTimeToTrigger);
    CHECK(d0.target == &a);

    HandoverDecision d1 = m.decide(a, cells, ue, 0.5);
    CHECK(d1.reason == DecisionReason::WaitingTimeToTrigger);

    HandoverDecision d2 = m.decide(a, cells, ue, 1.0);
    CHECK(d2.reason == DecisionReason::SignalHandover);
    CHECK(d2.target == &b);
}

static void testTimeToTriggerRestartsWhenAdvantageDisappears() {
    Cell a = makeCell("Cell-A", 0.0);
    Cell b = makeCell("Cell-B", 150.0);
    std::vector<Cell*> cells = {&a, &b};
    HandoverManager m(3.0, 0.80, 30.0, 0.05, /*TTT=*/1.0);

    CHECK(m.decide(a, cells, ueAt(110.0), 0.0).reason ==
          DecisionReason::WaitingTimeToTrigger);

    // advantage vanishes -> timer must reset
    CHECK(m.decide(a, cells, ueAt(75.0), 0.5).reason ==
          DecisionReason::StayBestAvailable);

    // advantage returns; only 0.4 s since the restart -> still waiting
    CHECK(m.decide(a, cells, ueAt(110.0), 0.6).reason ==
          DecisionReason::WaitingTimeToTrigger);
    CHECK(m.decide(a, cells, ueAt(110.0), 1.0).reason ==
          DecisionReason::WaitingTimeToTrigger);

    // a full second after the restart -> handover
    CHECK(m.decide(a, cells, ueAt(110.0), 1.7).reason ==
          DecisionReason::SignalHandover);
}

static void testMinimumSignalFloor() {
    Cell a = makeCell("Cell-A", 0.0);
    Cell b = makeCell("Cell-B", 150.0);
    std::vector<Cell*> cells = {&a, &b};

    // Floor of 0 dBm: every cell in this model is weaker, so none is usable.
    HandoverManager m(3.0, 0.80, 30.0, 0.05, 0.0, /*minSignalDbm=*/0.0);
    CHECK(m.decide(a, cells, ueAt(110.0), 0.0).target == &a);

    a.setAvailable(false);
    CHECK(m.decide(a, cells, ueAt(110.0), 0.0).reason ==
          DecisionReason::NoServiceAvailable);
}

int main() {
    testHealthyCandidateTriggersHandover();
    testRejectsBadCandidates();
    testEqualSignalStaysPut();
    testFailoverWhenServingCellDown();
    testNoServiceWhenEverythingDown();
    testFailoverAcceptsDegradedCellIfNothingHealthy();
    testFailoverPrefersHealthyOverStrongerDegraded();
    testPicksStrongestEligibleOfSeveral();
    testTimeToTrigger();
    testTimeToTriggerRestartsWhenAdvantageDisappears();
    testMinimumSignalFloor();

    if (failures == 0) {
        std::cout << "All handover tests passed." << std::endl;
        return 0;
    }
    std::cerr << failures << " check(s) failed." << std::endl;
    return 1;
}