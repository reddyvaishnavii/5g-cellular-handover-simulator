#ifndef CELL_H
#define CELL_H

#include <string>

class Cell {
public:
    Cell(
        std::string id,
        double x,
        double y,
        double coverageRadius,
        double baseSignalDbm,
        double latencyMs,
        double packetLossRate,
        int capacity
    );

    std::string getId() const;

    double getX() const;
    double getY() const;

    double getCoverageRadius() const;
    double getBaseSignalDbm() const;
    double getLatencyMs() const;
    double getPacketLossRate() const;

    int getCapacity() const;
    int getConnectedUsers() const;

    void connectUser();
    void disconnectUser();

    double getUtilization() const;

    bool contains(double x, double y) const;

    // ADD THESE TWO LINES
    double distanceTo(double userX, double userY) const;
    double calculateSignalDbm(double userX, double userY) const;

    void setAvailable(bool available);
    bool isAvailable() const;

private:
    std::string id;

    double x;
    double y;

    double coverageRadius;
    double baseSignalDbm;

    double latencyMs;
    double packetLossRate;

    int capacity;
    int connectedUsers;

    bool available;
};

#endif