#include "ParticleSystem.h"
#include <cstdlib>
#include <cmath>

ParticleSystem::ParticleSystem() {
    m_particles.reserve(MAX_PARTICLES);
}

ParticleSystem::~ParticleSystem() {
}

void ParticleSystem::reset() {
    m_particles.clear();
    m_skidMarks.clear();
}

void ParticleSystem::addSkidMark(const Vec2& leftWheel, const Vec2& rightWheel) {
    if (m_skidMarks.size() >= MAX_SKID_MARKS) {
        m_skidMarks.pop_front();
    }
    SkidPoint sp;
    sp.p1 = leftWheel;
    sp.p2 = rightWheel;
    sp.alpha = 0.85f;
    m_skidMarks.push_back(sp);
}

void ParticleSystem::emitNitroFlame(const Vec2& exhaustPos, const Vec2& carDir, int carModel) {
    if (m_particles.size() >= MAX_PARTICLES) return;

    for (int i = 0; i < 3; ++i) {
        Particle p;
        p.pos = exhaustPos + Vec2(((rand() % 8) - 4.0f), ((rand() % 8) - 4.0f));

        // Eject backwards relative to car direction
        Vec2 backDir = Vec2(-carDir.x, -carDir.y);
        float spread = ((rand() % 40) - 20.0f) * 0.01745f;
        Vec2 flameDir = backDir.rotated(spread);
        float speed = 180.0f + (rand() % 140);
        p.vel = flameDir * speed;

        p.life = 0.18f + ((rand() % 12) / 100.0f);
        p.maxLife = p.life;
        p.size = 5.0f + (rand() % 7);
        p.type = 1; // Nitro flame

        if (carModel == 2) {
            // Neon Cyan / Magenta
            if (rand() % 2 == 0) p.color = ColorRGB(0, 240, 255);
            else p.color = ColorRGB(255, 0, 220);
        } else if (carModel == 1) {
            p.color = ColorRGB(60, 180, 255);
        } else {
            if (rand() % 3 == 0) p.color = ColorRGB(100, 220, 255);
            else if (rand() % 2 == 0) p.color = ColorRGB(255, 200, 30);
            else p.color = ColorRGB(255, 60, 20);
        }

        m_particles.push_back(p);
    }
}

void ParticleSystem::emitTireSmoke(const Vec2& wheelPos, float driftAmount) {
    if (m_particles.size() >= MAX_PARTICLES) return;

    int count = 1 + (int)(driftAmount * 2.5f);
    for (int i = 0; i < count; ++i) {
        Particle p;
        p.pos = wheelPos + Vec2(((rand() % 10) - 5.0f), ((rand() % 10) - 5.0f));
        p.vel = Vec2(((rand() % 50) - 25.0f), ((rand() % 50) - 25.0f));

        p.life = 0.35f + ((rand() % 20) / 100.0f);
        p.maxLife = p.life;
        p.size = 6.0f + (rand() % 12);
        p.type = 0; // Smoke

        unsigned char shade = 180 + (rand() % 65);
        p.color = ColorRGB(shade, shade, shade);
        m_particles.push_back(p);
    }
}

void ParticleSystem::emitCrashSparks(const Vec2& impactPos, int count) {
    for (int i = 0; i < count; ++i) {
        if (m_particles.size() >= MAX_PARTICLES) break;

        Particle p;
        p.pos = impactPos;
        float angle = ((rand() % 360) * 3.14159f) / 180.0f;
        float speed = 120.0f + (rand() % 300);
        p.vel = Vec2(std::cos(angle) * speed, std::sin(angle) * speed);

        p.life = 0.35f + ((rand() % 25) / 100.0f);
        p.maxLife = p.life;
        p.size = 3.0f + (rand() % 4);
        p.type = 2; // Spark

        if (rand() % 3 == 0) p.color = ColorRGB(255, 255, 255);
        else if (rand() % 2 == 0) p.color = ColorRGB(255, 220, 40);
        else p.color = ColorRGB(255, 70, 20);

        m_particles.push_back(p);
    }
}

void ParticleSystem::emitRain(int screenWidth, int screenHeight, int count) {
    for (int i = 0; i < count; ++i) {
        if (m_particles.size() >= MAX_PARTICLES) break;

        Particle p;
        p.pos = Vec2((float)(rand() % screenWidth), (float)(rand() % screenHeight));
        p.vel = Vec2(-60.0f + (rand() % 30), 450.0f + (rand() % 200));
        p.life = 0.5f + ((rand() % 20) / 100.0f);
        p.maxLife = p.life;
        p.size = 2.0f;
        p.type = 3; // Rain
        p.color = ColorRGB(180, 220, 255);
        m_particles.push_back(p);
    }
}

void ParticleSystem::update(float dt, bool isRaining, int screenWidth, int screenHeight) {
    for (size_t i = 0; i < m_particles.size(); ) {
        Particle& p = m_particles[i];
        p.life -= dt;

        if (p.life <= 0.0f) {
            m_particles[i] = m_particles.back();
            m_particles.pop_back();
            continue;
        }

        p.pos.x += p.vel.x * dt;
        p.pos.y += p.vel.y * dt;

        if (p.type == 0) {
            // Smoke expands and decelerates
            p.vel.x *= 0.94f;
            p.vel.y *= 0.94f;
            p.size += dt * 14.0f;
        } else if (p.type == 1) {
            p.size *= 0.91f;
        } else if (p.type == 2) {
            p.vel.x *= 0.95f;
            p.vel.y *= 0.95f;
        }

        ++i;
    }

    if (isRaining) {
        emitRain(screenWidth, screenHeight, 5);
    }
}

void ParticleSystem::render(HDC hdc, const Vec2& cameraPos) {
    // 1. Draw 2D Skid Marks on asphalt
    if (m_skidMarks.size() >= 2) {
        HPEN skidPen = CreatePen(PS_SOLID, 4, RGB(22, 22, 28));
        HGDIOBJ oldP = SelectObject(hdc, skidPen);

        for (size_t i = 1; i < m_skidMarks.size(); ++i) {
            const SkidPoint& s0 = m_skidMarks[i - 1];
            const SkidPoint& s1 = m_skidMarks[i];

            // Don't connect discontinuous skid marks
            if ((s1.p1 - s0.p1).lengthSq() < 2500.0f) {
                // Left track
                MoveToEx(hdc, (int)(s0.p1.x - cameraPos.x), (int)(s0.p1.y - cameraPos.y), NULL);
                LineTo(hdc, (int)(s1.p1.x - cameraPos.x), (int)(s1.p1.y - cameraPos.y));

                // Right track
                MoveToEx(hdc, (int)(s0.p2.x - cameraPos.x), (int)(s0.p2.y - cameraPos.y), NULL);
                LineTo(hdc, (int)(s1.p2.x - cameraPos.x), (int)(s1.p2.y - cameraPos.y));
            }
        }

        SelectObject(hdc, oldP);
        DeleteObject(skidPen);
    }

    // 2. Draw Particles
    for (const Particle& p : m_particles) {
        if (p.type == 3) {
            // Screen-space Rain streak
            HPEN hPen = CreatePen(PS_SOLID, 1, RGB(p.color.r, p.color.g, p.color.b));
            HGDIOBJ oldPen = SelectObject(hdc, hPen);
            MoveToEx(hdc, (int)p.pos.x, (int)p.pos.y, NULL);
            LineTo(hdc, (int)(p.pos.x + p.vel.x * 0.035f), (int)(p.pos.y + p.vel.y * 0.035f));
            SelectObject(hdc, oldPen);
            DeleteObject(hPen);
        } else {
            // World-space particle translated by camera
            int px = (int)(p.pos.x - cameraPos.x);
            int py = (int)(p.pos.y - cameraPos.y);
            int rad = (int)(p.size * 0.5f);
            if (rad < 1) rad = 1;

            HBRUSH hBrush = CreateSolidBrush(p.color.toCOLORREF());
            HPEN hPen = CreatePen(PS_SOLID, 1, p.color.toCOLORREF());
            HGDIOBJ oldBrush = SelectObject(hdc, hBrush);
            HGDIOBJ oldPen = SelectObject(hdc, hPen);

            Ellipse(hdc, px - rad, py - rad, px + rad, py + rad);

            SelectObject(hdc, oldBrush);
            SelectObject(hdc, oldPen);
            DeleteObject(hBrush);
            DeleteObject(hPen);
        }
    }
}
