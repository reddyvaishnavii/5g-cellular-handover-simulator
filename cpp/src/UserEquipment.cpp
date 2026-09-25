#include "UserEquipment.h"

#include <utility>

UserEquipment::UserEquipment(
    std::string id,
    double x,
    double y,
    double velocityX,
    double velocityY
)
    : id(std::move(id)),
      x(x),
      y(y),
      velocityX(velocityX),
      velocityY(velocityY) {
}

std::string UserEquipment::getId() const {
    return id;
}

double UserEquipment::getX() const {
    return x;
}

double UserEquipment::getY() const {
    return y;
}

double UserEquipment::getVelocityX() const {
    return velocityX;
}

double UserEquipment::getVelocityY() const {
    return velocityY;
}

void UserEquipment::move(double timeStep) {
    x += velocityX * timeStep;
    y += velocityY * timeStep;
}