#ifndef USER_EQUIPMENT_H
#define USER_EQUIPMENT_H

#include <string>

class UserEquipment {
public:
    UserEquipment(
        std::string id,
        double x,
        double y,
        double velocityX,
        double velocityY
    );

    std::string getId() const;

    double getX() const;
    double getY() const;

    double getVelocityX() const;
    double getVelocityY() const;

    void move(double timeStep);

private:
    std::string id;

    double x;
    double y;

    double velocityX;
    double velocityY;
};

#endif