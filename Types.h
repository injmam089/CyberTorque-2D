#pragma once
#ifndef TYPES_H
#define TYPES_H

#include <windows.h>
#include <string>
#include <vector>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// 2D Vector
struct Vec2 {
    float x = 0.0f;
    float y = 0.0f;

    Vec2() : x(0.0f), y(0.0f) {}
    Vec2(float _x, float _y) : x(_x), y(_y) {}

    Vec2 operator+(const Vec2& o) const { return Vec2(x + o.x, y + o.y); }
    Vec2 operator-(const Vec2& o) const { return Vec2(x - o.x, y - o.y); }
    Vec2 operator*(float s) const { return Vec2(x * s, y * s); }
    Vec2& operator+=(const Vec2& o) { x += o.x; y += o.y; return *this; }
    Vec2& operator-=(const Vec2& o) { x -= o.x; y -= o.y; return *this; }

    float length() const { return std::sqrt(x * x + y * y); }
    float lengthSq() const { return x * x + y * y; }

    Vec2 normalized() const {
        float l = length();
        if (l < 0.0001f) return Vec2(0.0f, 0.0f);
        return Vec2(x / l, y / l);
    }

    static float dot(const Vec2& a, const Vec2& b) {
        return a.x * b.x + a.y * b.y;
    }

    Vec2 rotated(float angleRad) const {
        float c = std::cos(angleRad);
        float s = std::sin(angleRad);
        return Vec2(x * c - y * s, x * s + y * c);
    }
};

// RGB Color structure
struct ColorRGB {
    unsigned char r = 0;
    unsigned char g = 0;
    unsigned char b = 0;

    ColorRGB() : r(0), g(0), b(0) {}
    ColorRGB(unsigned char _r, unsigned char _g, unsigned char _b) : r(_r), g(_g), b(_b) {}
    COLORREF toCOLORREF() const { return RGB(r, g, b); }

    static ColorRGB lerp(const ColorRGB& a, const ColorRGB& b, float t) {
        if (t < 0.0f) t = 0.0f;
        if (t > 1.0f) t = 1.0f;
        return ColorRGB(
            (unsigned char)(a.r + (b.r - a.r) * t),
            (unsigned char)(a.g + (b.g - a.g) * t),
            (unsigned char)(a.b + (b.b - a.b) * t)
        );
    }
};

// Weather Modes
enum class WeatherType {
    SUNNY = 0,
    SUNSET,
    CYBER_NIGHT,
    RAINY
};

// Game Modes
enum class GameMode {
    CAREER = 0,
    ENDLESS_TRAFFIC,
    TIME_ATTACK
};

// Game States
enum class GameState {
    MAIN_MENU = 0,
    GARAGE,
    PLAYING,
    PAUSED,
    STAGE_CLEAR,
    GAME_OVER
};

// Particle Struct
struct Particle {
    Vec2 pos;
    Vec2 vel;
    float life = 1.0f;
    float maxLife = 1.0f;
    float size = 4.0f;
    ColorRGB color;
    int type = 0; // 0 = smoke, 1 = nitro flame, 2 = spark, 3 = rain streak
};

// 2D Tire Skid Mark Point
struct SkidPoint {
    Vec2 p1; // Left wheel
    Vec2 p2; // Right wheel
    float alpha = 1.0f;
};

// Collectible / Roadside Nature & Prop Types
enum class PropType {
    NONE = 0,
    TREE_OAK_LARGE,
    TREE_OAK_MEDIUM,
    TREE_PINE,
    TREE_PALM,
    TREE_CHERRY_BLOSSOM,
    BUSH_GREEN,
    BUSH_FLOWER,
    ROCK_BOULDER,
    CHECKPOINT_GATE,
    FINISH_GATE,
    COIN_PICKUP,
    NITRO_PICKUP
};

struct WorldProp {
    PropType type = PropType::NONE;
    Vec2 pos;
    float width = 40.0f;
    float height = 40.0f;
    float scale = 1.0f;
    bool collected = false;
};

// Save Data Structure
struct SaveData {
    int credits = 1500;
    int selectedCar = 0;
    int unlockedCars[4] = { 1, 0, 0, 0 };
    int upgradeSpeed[4] = { 1, 1, 1, 1 };
    int upgradeAccel[4] = { 1, 1, 1, 1 };
    int upgradeHandling[4] = { 1, 1, 1, 1 };
    int upgradeNitro[4] = { 1, 1, 1, 1 };
    int paintColorIndex[4] = { 0, 1, 2, 3 };
    int highScore = 0;
    int highestStage = 1;
};

#endif // TYPES_H
