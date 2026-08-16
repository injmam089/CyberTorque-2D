#include "Car.h"
#include <algorithm>
#include <cmath>

Car::Car() {
    reset(0);
}

Car::~Car() {
}

void Car::setBodyColor(const ColorRGB& color) {
    m_bodyColor = color;
    m_underglowColor = ColorRGB(
        (unsigned char)((255 - color.r) / 2 + color.r / 2),
        (unsigned char)((255 - color.g) / 2 + 100),
        (unsigned char)(255 - color.b / 2)
    );
}

void Car::reset(int carTypeIndex) {
    m_carType = carTypeIndex;
    m_pos = Vec2(0.0f, 100.0f);
    m_vel = Vec2(0.0f, 0.0f);
    m_angle = 0.0f;
    m_speed = 0.0f;
    m_rpm = 0.0f;
    m_gear = 1;
    m_nitro = 100.0f;
    m_nitroActive = false;
    m_isDrifting = false;
    m_driftSlip = 0.0f;
    m_steerInput = 0.0f;
    m_health = 100.0f;
    m_crashed = false;
    m_crashTimer = 0.0f;
    m_invulnerableTimer = 0.0f;

    if (m_carType == 0) {
        m_carName = "Apex Falcon";
        m_maxSpeed = 1350.0f;
        m_accelRate = 880.0f;
        m_turnSpeed = 3.8f;
        m_driftGrip = 0.94f;
        m_maxNitro = 100.0f;
        m_nitroRechargeRate = 6.0f;
        m_width = 38.0f;
        m_height = 72.0f;
        m_bodyColor = ColorRGB(235, 45, 45);
        m_underglowColor = ColorRGB(255, 30, 100);
    } else if (m_carType == 1) {
        m_carName = "Viper GT";
        m_maxSpeed = 1520.0f;
        m_accelRate = 980.0f;
        m_turnSpeed = 3.5f;
        m_driftGrip = 0.88f;
        m_maxNitro = 110.0f;
        m_nitroRechargeRate = 5.0f;
        m_width = 40.0f;
        m_height = 74.0f;
        m_bodyColor = ColorRGB(255, 140, 0);
        m_underglowColor = ColorRGB(255, 180, 0);
    } else if (m_carType == 2) {
        m_carName = "Cyber Phantom";
        m_maxSpeed = 1680.0f;
        m_accelRate = 1150.0f;
        m_turnSpeed = 4.0f;
        m_driftGrip = 0.96f;
        m_maxNitro = 140.0f;
        m_nitroRechargeRate = 9.0f;
        m_width = 38.0f;
        m_height = 76.0f;
        m_bodyColor = ColorRGB(0, 240, 255);
        m_underglowColor = ColorRGB(0, 255, 230);
    } else {
        m_carName = "Titan Enforcer";
        m_maxSpeed = 1260.0f;
        m_accelRate = 800.0f;
        m_turnSpeed = 3.2f;
        m_driftGrip = 0.97f;
        m_maxNitro = 100.0f;
        m_nitroRechargeRate = 5.0f;
        m_width = 44.0f;
        m_height = 78.0f;
        m_bodyColor = ColorRGB(215, 175, 50);
        m_underglowColor = ColorRGB(255, 215, 0);
    }
}

void Car::applyUpgrades(const SaveData& saveData) {
    int carIdx = m_carType;
    if (carIdx < 0 || carIdx > 3) carIdx = 0;

    int lvlSpeed = saveData.upgradeSpeed[carIdx];
    int lvlAccel = saveData.upgradeAccel[carIdx];
    int lvlHandling = saveData.upgradeHandling[carIdx];
    int lvlNitro = saveData.upgradeNitro[carIdx];

    m_maxSpeed *= (1.0f + (lvlSpeed - 1) * 0.07f);
    m_accelRate *= (1.0f + (lvlAccel - 1) * 0.09f);
    m_turnSpeed *= (1.0f + (lvlHandling - 1) * 0.08f);
    m_maxNitro *= (1.0f + (lvlNitro - 1) * 0.14f);
    m_nitro = m_maxNitro;

    const ColorRGB palette[5] = {
        ColorRGB(235, 45, 45), ColorRGB(0, 235, 255), ColorRGB(50, 255, 80), ColorRGB(30, 30, 35), ColorRGB(225, 185, 45)
    };
    int colorIdx = saveData.paintColorIndex[carIdx];
    if (colorIdx >= 0 && colorIdx < 5) {
        setBodyColor(palette[colorIdx]);
    }
}

void Car::addNitro(float amount) {
    m_nitro = (std::min)(m_maxNitro, m_nitro + amount);
}

void Car::hitObstacle(float speedLossPercent) {
    if (m_invulnerableTimer > 0.0f) return;

    if (m_carType == 3) {
        speedLossPercent *= 0.4f;
    }

    m_vel = m_vel * (1.0f - speedLossPercent);
    m_speed = m_vel.length();
    m_invulnerableTimer = 0.8f;
}

void Car::crash() {
    if (m_crashed) return;
    m_crashed = true;
    m_crashTimer = 1.0f;
    m_vel = Vec2(0.0f, 0.0f);
    m_speed = 0.0f;
    m_nitroActive = false;
    m_isDrifting = false;
}

void Car::repair() {
    m_crashed = false;
    m_crashTimer = 0.0f;
    m_invulnerableTimer = 1.4f;
    m_angle = 0.0f;
    m_vel = Vec2(0.0f, m_maxSpeed * 0.35f);
    m_speed = m_vel.length();
}

void Car::getRearWheelPositions(Vec2& outLeft, Vec2& outRight) const {
    Vec2 fwd = Vec2(std::sin(m_angle), std::cos(m_angle));
    Vec2 right = Vec2(std::cos(m_angle), -std::sin(m_angle));
    Vec2 rearCenter = m_pos - fwd * (m_height * 0.35f);
    outLeft = rearCenter - right * (m_width * 0.45f);
    outRight = rearCenter + right * (m_width * 0.45f);
}

void Car::getExhaustPositions(Vec2& outLeft, Vec2& outRight) const {
    Vec2 fwd = Vec2(std::sin(m_angle), std::cos(m_angle));
    Vec2 right = Vec2(std::cos(m_angle), -std::sin(m_angle));
    Vec2 rearEnd = m_pos - fwd * (m_height * 0.48f);
    outLeft = rearEnd - right * (m_width * 0.28f);
    outRight = rearEnd + right * (m_width * 0.28f);
}

void Car::getHeadlightPositions(Vec2& outLeft, Vec2& outRight) const {
    Vec2 fwd = Vec2(std::sin(m_angle), std::cos(m_angle));
    Vec2 right = Vec2(std::cos(m_angle), -std::sin(m_angle));
    Vec2 frontEnd = m_pos + fwd * (m_height * 0.46f);
    outLeft = frontEnd - right * (m_width * 0.32f);
    outRight = frontEnd + right * (m_width * 0.32f);
}

void Car::update(float dt, bool keyUp, bool keyDown, bool keyLeft, bool keyRight, bool keyBrake, bool keyNitro, float roadCenterX, float roadWidth) {
    if (m_invulnerableTimer > 0.0f) {
        m_invulnerableTimer -= dt;
        if (m_invulnerableTimer < 0.0f) m_invulnerableTimer = 0.0f;
    }

    if (m_crashed) {
        m_crashTimer -= dt;
        if (m_crashTimer <= 0.0f) {
            repair();
        }
        return;
    }

    // Nitro Overdrive with Smooth Acceleration
    float curMaxSpeed = m_maxSpeed;
    float curAccel = m_accelRate;

    if (keyNitro && m_nitro > 2.0f && m_speed > 150.0f) {
        m_nitroActive = true;
        m_nitro -= 22.0f * dt;
        curMaxSpeed *= 1.36f;
        curAccel *= 2.0f;
        if (m_nitro <= 0.0f) {
            m_nitro = 0.0f;
            m_nitroActive = false;
        }
    } else {
        m_nitroActive = false;
        if (m_speed > 250.0f) {
            m_nitro = (std::min)(m_maxNitro, m_nitro + m_nitroRechargeRate * dt);
        }
    }

    // 1. BUTTER-SMOOTH FORWARD THROTTLE & INERTIA
    if (keyUp) {
        if (m_vel.y < curMaxSpeed) {
            float throttleBoost = (1.0f - (m_vel.y / curMaxSpeed) * 0.4f);
            m_vel.y += curAccel * throttleBoost * dt;
            if (m_vel.y > curMaxSpeed) m_vel.y = curMaxSpeed;
        }
    } else if (keyDown) {
        if (m_vel.y > 0.0f) {
            m_vel.y -= m_brakingRate * dt;
            if (m_vel.y < 0.0f) m_vel.y = 0.0f;
        }
    } else {
        // Natural rolling coasting friction
        if (m_vel.y > 0.0f) {
            m_vel.y -= 240.0f * dt;
            if (m_vel.y < 0.0f) m_vel.y = 0.0f;
        }
    }

    if (keyBrake) {
        m_vel.y -= m_brakingRate * 0.75f * dt;
        if (m_vel.y < 0.0f) m_vel.y = 0.0f;
    }

    // 2. BUTTER-SMOOTH LATERAL STEERING & DAMPED ACCELERATION
    float targetSteer = 0.0f;
    if (keyLeft) targetSteer -= 1.0f;
    if (keyRight) targetSteer += 1.0f;

    m_steerInput += (targetSteer - m_steerInput) * 14.0f * dt;

    float speedFactor = (m_vel.y / m_maxSpeed);
    float targetLateralVel = m_steerInput * m_turnSpeed * 135.0f * (0.35f + speedFactor * 0.65f);
    m_vel.x += (targetLateralVel - m_vel.x) * 12.0f * dt;

    // 3. BUTTER-SMOOTH BODY ROTATION / LEAN ANGLE
    float targetAngle = m_steerInput * 0.22f;
    if (keyBrake && std::abs(m_steerInput) > 0.1f) {
        targetAngle = m_steerInput * 0.48f;
        m_isDrifting = true;
        m_nitro = (std::min)(m_maxNitro, m_nitro + 8.0f * dt);
    } else {
        if (std::abs(m_steerInput) < 0.12f) {
            m_isDrifting = false;
        }
    }
    m_angle += (targetAngle - m_angle) * 10.0f * dt;

    m_speed = m_vel.length();

    // Offroad grass slowdown
    float halfRoad = roadWidth * 0.5f;
    if (std::abs(m_pos.x) > halfRoad && m_vel.y > curMaxSpeed * 0.35f) {
        m_vel.y -= m_offroadDecel * dt;
    }

    // Integrate Position
    m_pos.x += m_vel.x * dt;
    m_pos.y += m_vel.y * dt;

    // Road boundaries with smooth rebound
    if (m_pos.x < -halfRoad - 35.0f) {
        m_pos.x = -halfRoad - 35.0f;
        m_vel.x = 0.0f;
    }
    if (m_pos.x > halfRoad + 35.0f) {
        m_pos.x = halfRoad + 35.0f;
        m_vel.x = 0.0f;
    }

    // Transmission & RPM
    float topRatio = m_speed / m_maxSpeed;
    if (topRatio < 0.18f) { m_gear = 1; m_rpm = topRatio / 0.18f; }
    else if (topRatio < 0.38f) { m_gear = 2; m_rpm = (topRatio - 0.18f) / 0.20f; }
    else if (topRatio < 0.62f) { m_gear = 3; m_rpm = (topRatio - 0.38f) / 0.24f; }
    else if (topRatio < 0.82f) { m_gear = 4; m_rpm = (topRatio - 0.62f) / 0.20f; }
    else { m_gear = 5; m_rpm = (topRatio - 0.82f) / 0.18f; if (m_rpm > 1.05f) m_rpm = 1.05f; }
}
