#include "Game.h"
#include <fstream>
#include <algorithm>
#include <cmath>

Game::Game() {
}

Game::~Game() {
    shutdown();
}

bool Game::init(HWND hwnd, int width, int height) {
    m_hwnd = hwnd;
    loadProfile();

    if (!m_renderer.init(hwnd, width, height)) {
        return false;
    }

    AudioSynth::getInstance().init();
    m_playerCar.reset(m_saveData.selectedCar);
    m_playerCar.applyUpgrades(m_saveData);

    return true;
}

void Game::resize(int width, int height) {
    m_renderer.resize(width, height);
}

void Game::shutdown() {
    saveProfile();
    AudioSynth::getInstance().shutdown();
    m_renderer.shutdown();
}

void Game::saveProfile() {
    std::ofstream out("savegame.dat", std::ios::binary);
    if (out.is_open()) {
        out.write(reinterpret_cast<const char*>(&m_saveData), sizeof(SaveData));
        out.close();
    }
}

void Game::loadProfile() {
    std::ifstream in("savegame.dat", std::ios::binary);
    if (in.is_open()) {
        in.read(reinterpret_cast<char*>(&m_saveData), sizeof(SaveData));
        in.close();
    } else {
        m_saveData.credits = 1500;
        m_saveData.selectedCar = 0;
        m_saveData.unlockedCars[0] = 1;
        m_saveData.unlockedCars[1] = 0;
        m_saveData.unlockedCars[2] = 0;
        m_saveData.unlockedCars[3] = 0;
        for (int i = 0; i < 4; ++i) {
            m_saveData.upgradeSpeed[i] = 1;
            m_saveData.upgradeAccel[i] = 1;
            m_saveData.upgradeHandling[i] = 1;
            m_saveData.upgradeNitro[i] = 1;
            m_saveData.paintColorIndex[i] = i % 5;
        }
        m_saveData.highScore = 0;
        m_saveData.highestStage = 1;
    }
}

void Game::startCareerGame(int stage) {
    m_mode = GameMode::CAREER;
    m_currentStage = stage;
    m_stageTime = 55.0f;
    m_score = 0;
    m_sessionCredits = 0;
    m_combo = 0;
    m_comboTimer = 0.0f;
    m_newHighScore = false;

    m_road.initTrack(m_currentStage, m_weather);
    m_traffic.initTraffic(28, 400.0f, m_road.getTrackLength(), m_road, false);
    m_particles.reset();

    m_playerCar.reset(m_saveData.selectedCar);
    m_playerCar.applyUpgrades(m_saveData);
    m_playerCar.setPos(Vec2(m_road.getRoadCenterX(100.0f), 100.0f));

    m_state = GameState::PLAYING;
}

void Game::startEndlessGame() {
    m_mode = GameMode::ENDLESS_TRAFFIC;
    m_currentStage = 0;
    m_stageTime = 60.0f;
    m_score = 0;
    m_sessionCredits = 0;
    m_combo = 0;
    m_comboTimer = 0.0f;
    m_newHighScore = false;

    m_road.initEndlessTrack();
    m_traffic.initTraffic(36, 400.0f, 25000.0f, m_road, true);
    m_particles.reset();

    m_playerCar.reset(m_saveData.selectedCar);
    m_playerCar.applyUpgrades(m_saveData);
    m_playerCar.setPos(Vec2(m_road.getRoadCenterX(100.0f), 100.0f));

    m_state = GameState::PLAYING;
}

void Game::startTimeAttack() {
    m_mode = GameMode::TIME_ATTACK;
    m_currentStage = 1;
    m_stageTime = 75.0f;
    m_score = 0;
    m_sessionCredits = 0;
    m_combo = 0;
    m_comboTimer = 0.0f;
    m_newHighScore = false;

    m_road.initTrack(1, m_weather);
    m_traffic.initTraffic(10, 400.0f, m_road.getTrackLength(), m_road, false);
    m_particles.reset();

    m_playerCar.reset(m_saveData.selectedCar);
    m_playerCar.applyUpgrades(m_saveData);
    m_playerCar.setPos(Vec2(m_road.getRoadCenterX(100.0f), 100.0f));

    m_state = GameState::PLAYING;
}

void Game::enterGarage() {
    m_garageCarIndex = m_saveData.selectedCar;
    m_garageStatIndex = 0;
    m_state = GameState::GARAGE;
}

void Game::enterMainMenu() {
    m_state = GameState::MAIN_MENU;
    m_menuIndex = 0;
}

void Game::addScore(int points) {
    m_score += points;
    if (m_score > m_saveData.highScore) {
        m_saveData.highScore = m_score;
        m_newHighScore = true;
    }
}

void Game::onKeyDown(WPARAM key) {
    if (m_state == GameState::PLAYING) {
        if (key == VK_UP || key == 'W') m_keyUp = true;
        if (key == VK_DOWN || key == 'S') m_keyDown = true;
        if (key == VK_LEFT || key == 'A') m_keyLeft = true;
        if (key == VK_RIGHT || key == 'D') m_keyRight = true;
        if (key == VK_SPACE) m_keyBrake = true;
        if (key == VK_SHIFT || key == 'N') m_keyNitro = true;

        if (key == 'M') {
            AudioSynth::getInstance().toggleMute();
        }

        if (key == VK_ESCAPE || key == 'P') {
            m_state = GameState::PAUSED;
            m_pauseMenuIndex = 0;
        }
    } else if (m_state == GameState::MAIN_MENU) {
        if (key == VK_UP || key == 'W') {
            m_menuIndex = (m_menuIndex + 4) % 5;
        } else if (key == VK_DOWN || key == 'S') {
            m_menuIndex = (m_menuIndex + 1) % 5;
        } else if (key == VK_RETURN || key == VK_SPACE) {
            if (m_menuIndex == 0) startCareerGame(1);
            else if (m_menuIndex == 1) startEndlessGame();
            else if (m_menuIndex == 2) startTimeAttack();
            else if (m_menuIndex == 3) enterGarage();
            else if (m_menuIndex == 4) PostQuitMessage(0);
        }
    } else if (m_state == GameState::GARAGE) {
        if (key == VK_LEFT || key == 'A') {
            m_garageCarIndex = (m_garageCarIndex + 3) % 4;
            if (m_saveData.unlockedCars[m_garageCarIndex]) {
                m_saveData.selectedCar = m_garageCarIndex;
            }
        } else if (key == VK_RIGHT || key == 'D') {
            m_garageCarIndex = (m_garageCarIndex + 1) % 4;
            if (m_saveData.unlockedCars[m_garageCarIndex]) {
                m_saveData.selectedCar = m_garageCarIndex;
            }
        } else if (key == VK_UP || key == 'W') {
            m_garageStatIndex = (m_garageStatIndex + 3) % 4;
        } else if (key == VK_DOWN || key == 'S') {
            m_garageStatIndex = (m_garageStatIndex + 1) % 4;
        } else if (key >= '1' && key <= '5') {
            int col = (int)(key - '1');
            m_saveData.paintColorIndex[m_garageCarIndex] = col;
            saveProfile();
        } else if (key == VK_RETURN || key == VK_SPACE) {
            const int unlockPrices[4] = { 0, 2500, 5000, 8000 };
            if (!m_saveData.unlockedCars[m_garageCarIndex]) {
                if (m_saveData.credits >= unlockPrices[m_garageCarIndex]) {
                    m_saveData.credits -= unlockPrices[m_garageCarIndex];
                    m_saveData.unlockedCars[m_garageCarIndex] = 1;
                    m_saveData.selectedCar = m_garageCarIndex;
                    AudioSynth::getInstance().triggerCoin();
                    saveProfile();
                }
            } else {
                int* upgArray[4] = {
                    &m_saveData.upgradeSpeed[m_garageCarIndex],
                    &m_saveData.upgradeAccel[m_garageCarIndex],
                    &m_saveData.upgradeHandling[m_garageCarIndex],
                    &m_saveData.upgradeNitro[m_garageCarIndex]
                };
                int curLvl = *upgArray[m_garageStatIndex];
                int cost = 800 * curLvl;
                if (curLvl < 5 && m_saveData.credits >= cost) {
                    m_saveData.credits -= cost;
                    (*upgArray[m_garageStatIndex])++;
                    AudioSynth::getInstance().triggerCoin();
                    saveProfile();
                }
            }
        } else if (key == VK_ESCAPE) {
            enterMainMenu();
        }
    } else if (m_state == GameState::PAUSED) {
        if (key == VK_UP || key == 'W') {
            m_pauseMenuIndex = (m_pauseMenuIndex + 2) % 3;
        } else if (key == VK_DOWN || key == 'S') {
            m_pauseMenuIndex = (m_pauseMenuIndex + 1) % 3;
        } else if (key == VK_RETURN || key == VK_SPACE) {
            if (m_pauseMenuIndex == 0) m_state = GameState::PLAYING;
            else if (m_pauseMenuIndex == 1) {
                if (m_mode == GameMode::CAREER) startCareerGame(m_currentStage);
                else if (m_mode == GameMode::ENDLESS_TRAFFIC) startEndlessGame();
                else startTimeAttack();
            } else if (m_pauseMenuIndex == 2) {
                enterMainMenu();
            }
        } else if (key == VK_ESCAPE) {
            m_state = GameState::PLAYING;
        }
    } else if (m_state == GameState::STAGE_CLEAR) {
        if (key == VK_RETURN || key == VK_SPACE) {
            if (m_currentStage < 4) {
                startCareerGame(m_currentStage + 1);
            } else {
                enterMainMenu();
            }
        }
    } else if (m_state == GameState::GAME_OVER) {
        if (key == VK_RETURN || key == VK_SPACE) {
            enterMainMenu();
        }
    }
}

void Game::onKeyUp(WPARAM key) {
    if (key == VK_UP || key == 'W') m_keyUp = false;
    if (key == VK_DOWN || key == 'S') m_keyDown = false;
    if (key == VK_LEFT || key == 'A') m_keyLeft = false;
    if (key == VK_RIGHT || key == 'D') m_keyRight = false;
    if (key == VK_SPACE) m_keyBrake = false;
    if (key == VK_SHIFT || key == 'N') m_keyNitro = false;
}

void Game::checkPickupsAndGates() {
    Vec2 pPos = m_playerCar.getPos();

    for (WorldProp& prop : m_road.getPropsRef()) {
        if (prop.collected) continue;

        if (prop.type == PropType::COIN_PICKUP) {
            if ((prop.pos - pPos).lengthSq() < 1600.0f) { // ~40px radius
                prop.collected = true;
                m_sessionCredits += 50;
                m_saveData.credits += 50;
                addScore(250);
                AudioSynth::getInstance().triggerCoin();
            }
        } else if (prop.type == PropType::NITRO_PICKUP) {
            if ((prop.pos - pPos).lengthSq() < 1600.0f) {
                prop.collected = true;
                m_playerCar.addNitro(35.0f);
                addScore(150);
                AudioSynth::getInstance().triggerNitroPickup();
            }
        } else if (prop.type == PropType::CHECKPOINT_GATE) {
            if (std::abs(prop.pos.y - pPos.y) < 30.0f && std::abs(prop.pos.x - pPos.x) < prop.width * 0.5f) {
                prop.collected = true;
                m_stageTime += 30.0f;
                addScore(2000);
                m_sessionCredits += 200;
                m_saveData.credits += 200;
                AudioSynth::getInstance().triggerCheckpoint();
                m_road.markCheckpointPassed();
            }
        } else if (prop.type == PropType::FINISH_GATE) {
            if (std::abs(prop.pos.y - pPos.y) < 35.0f && std::abs(prop.pos.x - pPos.x) < prop.width * 0.5f) {
                prop.collected = true;
                m_sessionCredits += 1000;
                m_saveData.credits += 1000;
                addScore(5000);
                saveProfile();
                m_state = GameState::STAGE_CLEAR;
                AudioSynth::getInstance().triggerCheckpoint();
            }
        }
    }
}

void Game::updatePlaying(float dt) {
    m_stageTime -= dt;
    if (m_stageTime <= 0.0f) {
        m_stageTime = 0.0f;
        saveProfile();
        m_state = GameState::GAME_OVER;
        return;
    }

    float roadCX = m_road.getRoadCenterX(m_playerCar.getPos().y);
    m_playerCar.update(dt, m_keyUp, m_keyDown, m_keyLeft, m_keyRight, m_keyBrake, m_keyNitro, roadCX, m_road.getRoadWidth());

    // Drifting tire marks & smoke
    if (m_playerCar.isDrifting() || (m_keyBrake && m_playerCar.getSpeed() > 200.0f)) {
        Vec2 wL, wR;
        m_playerCar.getRearWheelPositions(wL, wR);
        m_particles.addSkidMark(wL, wR);
        m_particles.emitTireSmoke(wL, m_playerCar.getDriftSlip());
        m_particles.emitTireSmoke(wR, m_playerCar.getDriftSlip());
        AudioSynth::getInstance().setDriftScreech(m_playerCar.getDriftSlip());
    } else {
        AudioSynth::getInstance().setDriftScreech(0.0f);
    }

    // Nitro exhaust fire
    if (m_playerCar.isNitroActive()) {
        Vec2 exL, exR;
        m_playerCar.getExhaustPositions(exL, exR);
        Vec2 fwd(std::sin(m_playerCar.getAngle()), std::cos(m_playerCar.getAngle()));
        m_particles.emitNitroFlame(exL, fwd, m_playerCar.getCarType());
        m_particles.emitNitroFlame(exR, fwd, m_playerCar.getCarType());
        m_screenShakeIntensity = (std::max)(m_screenShakeIntensity, 2.5f);
    }

    // Traffic AI update
    m_traffic.update(dt, m_playerCar.getPos(), m_playerCar.getSpeed(), m_road);

    // Collisions
    TrafficCar* hitCar = nullptr;
    if (m_traffic.checkCollision(m_playerCar.getPos(), m_playerCar.getWidth(), m_playerCar.getHeight(), m_playerCar.getAngle(), hitCar)) {
        if (!m_playerCar.isCrashed() && m_playerCar.getInvulnerableTimer() <= 0.0f) {
            m_playerCar.crash();
            m_screenShakeIntensity = 16.0f;
            m_combo = 0;
            AudioSynth::getInstance().triggerCrash();
            m_particles.emitCrashSparks(m_playerCar.getPos(), 35);
        }
    }

    // Near-Miss Combo
    TrafficCar* missedCar = nullptr;
    if (m_traffic.checkNearMiss(m_playerCar.getPos(), m_playerCar.getSpeed(), missedCar)) {
        m_combo++;
        m_comboTimer = 2.0f;
        int pts = 500 * m_combo;
        addScore(pts);
        m_sessionCredits += 25 * m_combo;
        m_saveData.credits += 25 * m_combo;
        AudioSynth::getInstance().triggerCoin();
    }

    if (m_comboTimer > 0.0f) {
        m_comboTimer -= dt;
        if (m_comboTimer <= 0.0f) m_combo = 0;
    }

    // Pickups and checkpoints
    checkPickupsAndGates();

    if (m_playerCar.getSpeed() > 100.0f) {
        addScore((int)(m_playerCar.getSpeed() * dt * 0.1f));
    }

    // Update Particles
    m_particles.update(dt, (m_weather == WeatherType::RAINY), m_renderer.getWidth(), m_renderer.getHeight());

    // Audio Engine synchronization
    float rpmRatio = m_playerCar.getRpm();
    AudioSynth::getInstance().setEngineRPM(rpmRatio, m_keyUp);
    AudioSynth::getInstance().setNitroActive(m_playerCar.isNitroActive());
    AudioSynth::getInstance().triggerPoliceSiren(m_traffic.isPoliceActive());

    // Screen Shake decay
    if (m_screenShakeIntensity > 0.0f) {
        m_screenShakeX = ((rand() % 200) - 100) / 100.0f * m_screenShakeIntensity;
        m_screenShakeY = ((rand() % 200) - 100) / 100.0f * m_screenShakeIntensity;
        m_screenShakeIntensity -= dt * 30.0f;
        if (m_screenShakeIntensity < 0.0f) {
            m_screenShakeIntensity = 0.0f;
            m_screenShakeX = 0.0f;
            m_screenShakeY = 0.0f;
        }
    }
}

void Game::update(float dt) {
    if (m_state == GameState::PLAYING) {
        updatePlaying(dt);
    } else {
        AudioSynth::getInstance().setEngineRPM(0.0f, false);
        AudioSynth::getInstance().setNitroActive(false);
        AudioSynth::getInstance().setDriftScreech(0.0f);
        AudioSynth::getInstance().triggerPoliceSiren(false);
    }
}

void Game::render(HDC destHdc) {
    m_renderer.beginFrame();

    if (m_state == GameState::PLAYING || m_state == GameState::PAUSED || m_state == GameState::STAGE_CLEAR || m_state == GameState::GAME_OVER) {
        m_renderer.renderWorld(m_road, m_playerCar, m_traffic, m_particles, m_weather, m_screenShakeX, m_screenShakeY);
        m_renderer.renderHUD(m_playerCar, m_road, m_stageTime, m_score, m_combo, m_comboTimer, AudioSynth::getInstance().isMuted(), m_mode);

        if (m_state == GameState::PAUSED) {
            m_renderer.renderPauseMenu(m_pauseMenuIndex);
        } else if (m_state == GameState::STAGE_CLEAR) {
            m_renderer.renderStageClear(m_currentStage, m_score, m_sessionCredits, 60.0f - m_stageTime);
        } else if (m_state == GameState::GAME_OVER) {
            m_renderer.renderGameOver(m_score, m_saveData.highScore, m_sessionCredits, m_newHighScore);
        }
    } else if (m_state == GameState::MAIN_MENU) {
        m_renderer.renderMainMenu(m_menuIndex, m_saveData.highScore, m_saveData.credits);
    } else if (m_state == GameState::GARAGE) {
        m_renderer.renderGarage(m_garageCarIndex, m_saveData, m_garageStatIndex, m_saveData.paintColorIndex[m_garageCarIndex]);
    }

    m_renderer.endFrame(destHdc);
}
