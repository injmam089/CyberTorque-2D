#pragma once
#ifndef PARTICLE_SYSTEM_H
#define PARTICLE_SYSTEM_H

#include "Types.h"
#include <vector>
#include <deque>

class ParticleSystem {
public:
    ParticleSystem();
    ~ParticleSystem();

    void update(float dt, bool isRaining, int screenWidth, int screenHeight);
    void render(HDC hdc, const Vec2& cameraPos);
    void reset();

    // Spawners
    void emitNitroFlame(const Vec2& exhaustPos, const Vec2& carDir, int carModel);
    void emitTireSmoke(const Vec2& wheelPos, float driftAmount);
    void emitCrashSparks(const Vec2& impactPos, int count);
    void emitRain(int screenWidth, int screenHeight, int count);
    void addSkidMark(const Vec2& leftWheel, const Vec2& rightWheel);

private:
    std::vector<Particle> m_particles;
    std::deque<SkidPoint> m_skidMarks;
    static const int MAX_PARTICLES = 800;
    static const int MAX_SKID_MARKS = 350;
};

#endif // PARTICLE_SYSTEM_H
