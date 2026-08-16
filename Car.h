#pragma once
#ifndef CAR_H
#define CAR_H

#include "Types.h"
#include <string>

class Car {
public:
    Car();
    ~Car();

    void reset(int carTypeIndex = 0);
    void applyUpgrades(const SaveData& saveData);
    void update(float dt, bool keyUp, bool keyDown, bool keyLeft, bool keyRight, bool keyBrake, bool keyNitro, float roadCenterX, float roadWidth);

    // Getters
    Vec2 getPos() const { return m_pos; }
    Vec2 getVel() const { return m_vel; }
    float getAngle() const { return m_angle; }
    float getSpeed() const { return m_speed; }
    float getMaxSpeed() const { return m_maxSpeed; }
    float getSpeedMph() const { return m_speed * 0.16f; }
    float getRpm() const { return m_rpm; }
    int getGear() const { return m_gear; }
    float getNitro() const { return m_nitro; }
    bool isNitroActive() const { return m_nitroActive; }
    bool isDrifting() const { return m_isDrifting; }
    float getDriftSlip() const { return m_driftSlip; }
    int getCarType() const { return m_carType; }
    std::string getCarName() const { return m_carName; }
    ColorRGB getBodyColor() const { return m_bodyColor; }
    ColorRGB getUnderglowColor() const { return m_underglowColor; }
    float getHealth() const { return m_health; }
    bool isCrashed() const { return m_crashed; }
    float getInvulnerableTimer() const { return m_invulnerableTimer; }

    float getWidth() const { return m_width; }
    float getHeight() const { return m_height; }

    // Wheel positions in world space (for skid marks & smoke)
    void getRearWheelPositions(Vec2& outLeft, Vec2& outRight) const;
    void getExhaustPositions(Vec2& outLeft, Vec2& outRight) const;
    void getHeadlightPositions(Vec2& outLeft, Vec2& outRight) const;

    // Modifiers & Actions
    void setPos(const Vec2& pos) { m_pos = pos; }
    void addNitro(float amount);
    void hitObstacle(float speedLossPercent = 0.4f);
    void crash();
    void repair();
    void setBodyColor(const ColorRGB& color);

private:
    int m_carType = 0;
    std::string m_carName = "Apex Falcon";
    ColorRGB m_bodyColor = ColorRGB(235, 45, 45);
    ColorRGB m_underglowColor = ColorRGB(255, 40, 120);

    // 2D Physics state
    Vec2 m_pos = Vec2(0.0f, 100.0f);
    Vec2 m_vel = Vec2(0.0f, 0.0f);
    float m_angle = 0.0f; // 0 = straight up (+Y)
    float m_speed = 0.0f;

    float m_width = 38.0f;
    float m_height = 72.0f;

    float m_maxSpeed = 1350.0f;
    float m_accelRate = 850.0f;
    float m_brakingRate = 1600.0f;
    float m_turnSpeed = 3.6f;
    float m_driftGrip = 0.92f;
    float m_offroadDecel = 900.0f;

    // RPM & Transmission
    float m_rpm = 0.0f;
    int m_gear = 1;

    // Nitro system
    float m_nitro = 100.0f;
    float m_maxNitro = 100.0f;
    bool m_nitroActive = false;
    float m_nitroRechargeRate = 6.0f;

    // Drifting & Slip
    bool m_isDrifting = false;
    float m_driftSlip = 0.0f;
    float m_steerInput = 0.0f;

    // Health & Collisions
    float m_health = 100.0f;
    bool m_crashed = false;
    float m_crashTimer = 0.0f;
    float m_invulnerableTimer = 0.0f;
};

#endif // CAR_H
