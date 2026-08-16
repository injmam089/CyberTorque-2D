#pragma once
#ifndef TRAFFIC_H
#define TRAFFIC_H

#include "Types.h"
#include "Road.h"
#include <vector>

enum class TrafficType {
    SEDAN = 0,
    SPORTS_CAR,
    SEMI_TRUCK,
    POLICE_CRUISER
};

struct TrafficCar {
    TrafficType type = TrafficType::SEDAN;
    Vec2 pos;
    Vec2 vel;
    float angle = 0.0f;
    float speed = 600.0f;
    float targetLaneOffset = 0.0f;
    float laneChangeTimer = 0.0f;
    float width = 36.0f;
    float height = 68.0f;
    ColorRGB color;
    float sirenTimer = 0.0f;
    bool nearMissAwarded = false;
};

class Traffic {
public:
    Traffic();
    ~Traffic();

    void initTraffic(int count, float startY, float endY, const Road& road, bool spawnPolice = false);
    void update(float dt, const Vec2& playerPos, float playerSpeed, const Road& road);

    bool checkCollision(const Vec2& playerPos, float pWidth, float pHeight, float pAngle, TrafficCar*& outHitCar);
    bool checkNearMiss(const Vec2& playerPos, float pSpeed, TrafficCar*& outMissedCar);

    const std::vector<TrafficCar>& getCars() const { return m_cars; }
    std::vector<TrafficCar>& getCarsRef() { return m_cars; }
    bool isPoliceActive() const { return m_policeActive; }

private:
    std::vector<TrafficCar> m_cars;
    bool m_policeActive = false;
};

#endif // TRAFFIC_H
