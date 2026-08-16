#include "Traffic.h"
#include <cstdlib>
#include <cmath>
#include <algorithm>

Traffic::Traffic() {
}

Traffic::~Traffic() {
}

void Traffic::initTraffic(int count, float startY, float endY, const Road& road, bool spawnPolice) {
    m_cars.clear();
    m_policeActive = false;

    if (count <= 0) return;

    float spacing = (endY - startY) / (float)count;
    float laneWidth = road.getRoadWidth() / 4.0f;
    const float laneOffsets[4] = { -1.5f * laneWidth, -0.5f * laneWidth, 0.5f * laneWidth, 1.5f * laneWidth };

    const ColorRGB trafficColors[6] = {
        ColorRGB(235, 235, 240), // Pearl White
        ColorRGB(45, 48, 55),    // Gloss Charcoal
        ColorRGB(35, 110, 225),  // Cobalt Blue
        ColorRGB(225, 185, 25),  // Metallic Gold
        ColorRGB(200, 35, 40),   // Crimson Red
        ColorRGB(35, 165, 75)    // Emerald Green
    };

    for (int i = 0; i < count; ++i) {
        TrafficCar car;
        int laneIdx = i % 4;
        car.targetLaneOffset = laneOffsets[laneIdx];
        float y = startY + (float)i * spacing + ((rand() % 80) - 40.0f);
        car.pos = Vec2(car.targetLaneOffset, y);
        car.angle = 0.0f;
        car.laneChangeTimer = 4.0f + ((rand() % 60) / 10.0f);
        car.nearMissAwarded = false;

        int rType = rand() % 10;
        if (spawnPolice && i == count - 1) {
            car.type = TrafficType::POLICE_CRUISER;
            car.speed = 1020.0f;
            car.color = ColorRGB(20, 20, 25);
            car.width = 38.0f;
            car.height = 74.0f;
            m_policeActive = true;
        } else if (rType < 2) {
            car.type = TrafficType::SEMI_TRUCK;
            car.speed = 460.0f + (rand() % 80);
            car.color = ColorRGB(170, 55, 45);
            car.width = 46.0f;
            car.height = 110.0f;
        } else if (rType < 5) {
            car.type = TrafficType::SPORTS_CAR;
            car.speed = 760.0f + (rand() % 160);
            car.color = trafficColors[rand() % 6];
            car.width = 36.0f;
            car.height = 70.0f;
        } else {
            car.type = TrafficType::SEDAN;
            car.speed = 580.0f + (rand() % 120);
            car.color = trafficColors[rand() % 6];
            car.width = 36.0f;
            car.height = 68.0f;
        }

        m_cars.push_back(car);
    }
}

void Traffic::update(float dt, const Vec2& playerPos, float playerSpeed, const Road& road) {
    float laneWidth = road.getRoadWidth() / 4.0f;
    const float laneOffsets[4] = { -1.5f * laneWidth, -0.5f * laneWidth, 0.5f * laneWidth, 1.5f * laneWidth };
    bool hasPoliceNear = false;

    for (size_t i = 0; i < m_cars.size(); ++i) {
        TrafficCar& car = m_cars[i];

        // 1. POLICE PURSUIT SMOOTH CHASING
        if (car.type == TrafficType::POLICE_CRUISER) {
            car.sirenTimer += dt * 8.0f;
            if (car.sirenTimer > 10.0f) car.sirenTimer -= 10.0f;

            float distToPlayer = (car.pos - playerPos).length();
            if (distToPlayer < 1200.0f) {
                hasPoliceNear = true;
            }

            if (car.pos.y < playerPos.y) {
                float desiredSpeed = (std::max)(car.speed, playerSpeed * 1.03f);
                car.speed += (desiredSpeed - car.speed) * 2.0f * dt;
                car.targetLaneOffset = playerPos.x;
            }
        }

        // 2. SMOOTH CRUISE SPEED & FORWARD POSITIONING
        car.angle = 0.0f;
        car.pos.y += car.speed * dt;

        // 3. BUTTER-SMOOTH LATERAL LANE BLENDING (Spring-damper curve)
        float dx = car.targetLaneOffset - car.pos.x;
        car.pos.x += dx * 2.8f * dt;

        // 4. PERIODIC SAFE LANE CHANGE DECISIONS
        car.laneChangeTimer -= dt;
        if (car.laneChangeTimer <= 0.0f && car.type != TrafficType::POLICE_CRUISER) {
            car.laneChangeTimer = 5.0f + ((rand() % 60) / 10.0f);
            int newLane = rand() % 4;
            car.targetLaneOffset = laneOffsets[newLane];
        }

        // 5. RESET NEAR-MISS FLAG WHEN DISTANT
        if (std::abs(car.pos.y - playerPos.y) > 1200.0f) {
            car.nearMissAwarded = false;
        }

        // 6. CONTINUOUS SEAMLESS TRAFFIC RESPAWNING AROUND PLAYER
        if (car.pos.y < playerPos.y - 1200.0f && car.type != TrafficType::POLICE_CRUISER) {
            // Reposition ahead of player
            car.pos.y = playerPos.y + 2000.0f + (rand() % 600);
            int laneIdx = rand() % 4;
            car.targetLaneOffset = laneOffsets[laneIdx];
            car.pos.x = car.targetLaneOffset;
            car.nearMissAwarded = false;
        }
    }

    m_policeActive = hasPoliceNear;
}

bool Traffic::checkCollision(const Vec2& playerPos, float pWidth, float pHeight, float pAngle, TrafficCar*& outHitCar) {
    for (TrafficCar& car : m_cars) {
        float dy = std::abs(car.pos.y - playerPos.y);
        if (dy < (car.height + pHeight) * 0.44f) {
            float dx = std::abs(car.pos.x - playerPos.x);
            if (dx < (car.width + pWidth) * 0.44f) {
                outHitCar = &car;
                return true;
            }
        }
    }
    outHitCar = nullptr;
    return false;
}

bool Traffic::checkNearMiss(const Vec2& playerPos, float pSpeed, TrafficCar*& outMissedCar) {
    if (pSpeed < 450.0f) return false;

    for (TrafficCar& car : m_cars) {
        if (car.nearMissAwarded) continue;

        float dy = std::abs(car.pos.y - playerPos.y);
        if (dy < (car.height + 70.0f) * 0.48f) {
            float dx = std::abs(car.pos.x - playerPos.x);
            if (dx >= car.width * 0.44f && dx <= car.width * 1.5f) {
                car.nearMissAwarded = true;
                outMissedCar = &car;
                return true;
            }
        }
    }
    outMissedCar = nullptr;
    return false;
}
