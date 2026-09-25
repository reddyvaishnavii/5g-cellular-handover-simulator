#include "Cell.h"

#include <cmath>
#include <utility>

Cell::Cell(
    std::string id,
    double x,
    double y,
    double coverageRadius,
    double baseSignalDbm,
    double latencyMs,
    double packetLossRate,
    int capacity
)
    : id(std::move(id)),
      x(x),
      y(y),
      coverageRadius(coverageRadius),
      baseSignalDbm(baseSignalDbm),
      latencyMs(latencyMs),
      packetLossRate(packetLossRate),
      capacity(capacity),
      connectedUsers(0),
      available(true) {
}

std::string Cell::getId() const {
    return id;
}

double Cell::getX() const {
    return x;
}

double Cell::getY() const {
    return y;
}

double Cell::getCoverageRadius() const {
    return coverageRadius;
}

double Cell::getBaseSignalDbm() const {
    return baseSignalDbm;
}

double Cell::getLatencyMs() const {
    return latencyMs;
}

double Cell::getPacketLossRate() const {
    return packetLossRate;
}

int Cell::getCapacity() const {
    return capacity;
}

int Cell::getConnectedUsers() const {
    return connectedUsers;
}

bool Cell::contains(double userX, double userY) const {
    double dx = userX - x;
    double dy = userY - y;

    double distance = std::sqrt(dx * dx + dy * dy);

    return distance <= coverageRadius;
}

double Cell::distanceTo(double userX, double userY) const {
    double dx = userX - x;
    double dy = userY - y;

    return std::sqrt(dx * dx + dy * dy);
}

double Cell::calculateSignalDbm(double userX, double userY) const {
    double distance = distanceTo(userX, userY);

    if (distance == 0.0) {
        return baseSignalDbm;
    }

    return baseSignalDbm - (20.0 * std::log10(distance));
}

void Cell::connectUser() {
    if (connectedUsers < capacity) {
        connectedUsers++;
    }
}

void Cell::disconnectUser() {
    if (connectedUsers > 0) {
        connectedUsers--;
    }
}

double Cell::getUtilization() const {
    if (capacity == 0) {
        return 1.0;
    }

    return static_cast<double>(connectedUsers) / capacity;
}

void Cell::setAvailable(bool available) {
    this->available = available;
}

bool Cell::isAvailable() const {
    return available;
}