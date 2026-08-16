#pragma once
#ifndef RENDERER_H
#define RENDERER_H

#include "Types.h"
#include "Road.h"
#include "Car.h"
#include "Traffic.h"
#include "ParticleSystem.h"
#include <windows.h>
#include <string>
#include <vector>

class Renderer {
public:
    Renderer();
    ~Renderer();

    bool init(HWND hwnd, int width, int height);
    void resize(int width, int height);
    void shutdown();

    void beginFrame();
    void endFrame(HDC destHdc);

    // Main 2D rendering passes
    void renderWorld(const Road& road, const Car& car, const Traffic& traffic, ParticleSystem& particles, WeatherType weather, float screenShakeX, float screenShakeY);
    void renderHUD(const Car& car, const Road& road, float stageTime, int score, int combo, float comboTimer, bool isMuted, GameMode mode);
    
    // UI Screen Rendering
    void renderMainMenu(int selectedItem, int highScore, int credits);
    void renderGarage(int selectedCar, const SaveData& save, int selectedStat, int selectedColor);
    void renderStageClear(int stage, int score, int rewardCredits, float timeTaken);
    void renderGameOver(int score, int highScore, int creditsEarned, bool newHighScore);
    void renderPauseMenu(int selectedItem);

    int getWidth() const { return m_width; }
    int getHeight() const { return m_height; }

private:
    // Upgraded Dedicated Vehicle Renderers
    void drawSupercar(const Vec2& screenPos, float angle, float width, float height, const ColorRGB& bodyColor, const ColorRGB& underglowColor, bool brakeLights, bool drawUnderglow = true);
    void drawMuscleCar(const Vec2& screenPos, float angle, float width, float height, const ColorRGB& bodyColor, const ColorRGB& underglowColor, bool brakeLights, bool drawUnderglow = true);
    void drawHypercar(const Vec2& screenPos, float angle, float width, float height, const ColorRGB& bodyColor, const ColorRGB& underglowColor, bool brakeLights, bool drawUnderglow = true);
    void drawTitanEnforcer(const Vec2& screenPos, float angle, float width, float height, const ColorRGB& bodyColor, const ColorRGB& underglowColor, bool brakeLights, bool drawUnderglow = true);
    void drawPoliceCruiser(const Vec2& screenPos, float angle, float width, float height, float sirenTimer);
    void drawSemiTruck(const Vec2& screenPos, float angle, float width, float height, const ColorRGB& cabColor);
    void drawTrafficSedan(const Vec2& screenPos, float angle, float width, float height, const ColorRGB& bodyColor);

    void drawHeadlightBeams(const Vec2& screenPos, float angle, float carWidth, float carHeight);
    void drawProp(const WorldProp& prop, const Vec2& cameraPos);
    void drawSpeedometerHUD(int x, int y, float speedMph, float maxSpeedMph, float rpm, int gear);
    void drawNitroGauge(int x, int y, int w, int h, float nitro, float maxNitro, bool active);

    HWND m_hwnd = nullptr;
    HDC m_hdcBack = nullptr;
    HBITMAP m_hbmBack = nullptr;
    HBITMAP m_hbmOld = nullptr;
    int m_width = 1024;
    int m_height = 768;

    Vec2 m_cameraPos;
    float m_animTick = 0.0f;
};

#endif // RENDERER_H
