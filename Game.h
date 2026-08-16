#pragma once
#ifndef GAME_H
#define GAME_H

#include "Types.h"
#include "Road.h"
#include "Car.h"
#include "Traffic.h"
#include "ParticleSystem.h"
#include "Renderer.h"
#include "AudioSynth.h"
#include <string>

class Game {
public:
    Game();
    ~Game();

    bool init(HWND hwnd, int width, int height);
    void resize(int width, int height);
    void shutdown();

    void update(float dt);
    void render(HDC destHdc);

    void onKeyDown(WPARAM key);
    void onKeyUp(WPARAM key);

    void startCareerGame(int stage = 1);
    void startEndlessGame();
    void startTimeAttack();
    void enterGarage();
    void enterMainMenu();

    void saveProfile();
    void loadProfile();

private:
    void updatePlaying(float dt);
    void checkPickupsAndGates();
    void addScore(int points);

    HWND m_hwnd = nullptr;
    Renderer m_renderer;
    Road m_road;
    Car m_playerCar;
    Traffic m_traffic;
    ParticleSystem m_particles;

    GameState m_state = GameState::MAIN_MENU;
    GameMode m_mode = GameMode::CAREER;
    WeatherType m_weather = WeatherType::SUNSET;

    SaveData m_saveData;

    float m_stageTime = 60.0f;
    int m_score = 0;
    int m_sessionCredits = 0;
    int m_combo = 0;
    float m_comboTimer = 0.0f;
    int m_currentStage = 1;
    bool m_newHighScore = false;

    // Screen Shake FX
    float m_screenShakeIntensity = 0.0f;
    float m_screenShakeX = 0.0f;
    float m_screenShakeY = 0.0f;

    // UI Menu selections
    int m_menuIndex = 0;
    int m_garageCarIndex = 0;
    int m_garageStatIndex = 0;
    int m_pauseMenuIndex = 0;

    // Key states
    bool m_keyUp = false;
    bool m_keyDown = false;
    bool m_keyLeft = false;
    bool m_keyRight = false;
    bool m_keyBrake = false;
    bool m_keyNitro = false;
};

#endif // GAME_H
