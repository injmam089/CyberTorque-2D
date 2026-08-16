#include "Road.h"
#include <cmath>
#include <cstdlib>

Road::Road() {
}

Road::~Road() {
}

float Road::getRoadCenterX(float worldY) const {
    return 0.0f; // Perfectly straight highway
}

float Road::getRoadAngle(float worldY) const {
    return 0.0f; // Straight forward
}

void Road::initTrack(int stageIndex, WeatherType& outWeather) {
    m_stage = stageIndex;
    m_totalLength = 28000.0f;
    m_roadWidth = 640.0f;
    m_checkpointsPassed = 0;

    if (stageIndex == 1) {
        m_stageName = "Pacific Coastline Parkway";
        outWeather = WeatherType::SUNSET;
        m_asphaltColor = ColorRGB(46, 50, 58);
        m_shoulderColor = ColorRGB(42, 148, 52); // Lush vibrant green meadow
        m_curbColor = ColorRGB(225, 45, 45);
        m_lineColor = ColorRGB(245, 245, 255);
    } else if (stageIndex == 2) {
        m_stageName = "Cyber Metropolis Forest";
        outWeather = WeatherType::CYBER_NIGHT;
        m_asphaltColor = ColorRGB(28, 30, 42);
        m_shoulderColor = ColorRGB(18, 90, 48); // Deep emerald cyber lawn
        m_curbColor = ColorRGB(0, 230, 255);
        m_lineColor = ColorRGB(0, 240, 255);
    } else if (stageIndex == 3) {
        m_stageName = "Alpine Mountain Pass";
        outWeather = WeatherType::SUNNY;
        m_asphaltColor = ColorRGB(55, 58, 65);
        m_shoulderColor = ColorRGB(50, 160, 60); // Alpine highland green
        m_curbColor = ColorRGB(255, 255, 255);
        m_lineColor = ColorRGB(255, 215, 0);
    } else {
        m_stageName = "Neo Tokyo Expressway";
        outWeather = WeatherType::RAINY;
        m_asphaltColor = ColorRGB(24, 28, 36);
        m_shoulderColor = ColorRGB(32, 120, 50);
        m_curbColor = ColorRGB(255, 220, 0);
        m_lineColor = ColorRGB(240, 240, 255);
    }

    generateProps(stageIndex);
}

void Road::initEndlessTrack() {
    m_stage = 0;
    m_stageName = "Endless Nature Highway";
    m_totalLength = 1000000.0f;
    m_roadWidth = 640.0f;
    m_checkpointsPassed = 0;

    m_asphaltColor = ColorRGB(38, 42, 50);
    m_shoulderColor = ColorRGB(45, 155, 55); // Rich green grass
    m_curbColor = ColorRGB(220, 40, 40);
    m_lineColor = ColorRGB(255, 255, 255);

    generateProps(0);
}

void Road::generateProps(int stage) {
    m_props.clear();

    float endY = (stage == 0) ? 60000.0f : m_totalLength;

    // Checkpoints every 4500 units
    for (float y = 4000.0f; y < endY - 2000.0f; y += 4500.0f) {
        WorldProp cp;
        cp.type = PropType::CHECKPOINT_GATE;
        cp.pos = Vec2(0.0f, y);
        cp.width = m_roadWidth + 80.0f;
        cp.height = 30.0f;
        m_props.push_back(cp);
    }

    // Finish gate at end
    if (stage > 0) {
        WorldProp fg;
        fg.type = PropType::FINISH_GATE;
        fg.pos = Vec2(0.0f, m_totalLength - 500.0f);
        fg.width = m_roadWidth + 80.0f;
        fg.height = 35.0f;
        m_props.push_back(fg);
    }

    // DENSE LUSH NATURE ON BOTH SIDES OF THE ROAD
    // We populate multiple rows of trees, bushes, and flowers along Left and Right
    float halfRoad = m_roadWidth * 0.5f;

    for (float y = 100.0f; y < endY; y += 65.0f) {
        // --- LEFT SIDE NATURE (Inner row, Mid row, Outer forest) ---
        // Inner Row (close to curb)
        WorldProp propL1;
        float xOffsetL1 = halfRoad + 45.0f + (rand() % 35);
        propL1.pos = Vec2(-xOffsetL1, y + ((rand() % 30) - 15.0f));
        propL1.scale = 0.85f + ((rand() % 35) / 100.0f);

        int rTypeL1 = rand() % 10;
        if (rTypeL1 < 5) propL1.type = PropType::TREE_OAK_LARGE;
        else if (rTypeL1 < 7) propL1.type = PropType::TREE_OAK_MEDIUM;
        else if (rTypeL1 < 8) propL1.type = PropType::TREE_CHERRY_BLOSSOM;
        else if (rTypeL1 < 9) propL1.type = PropType::BUSH_FLOWER;
        else propL1.type = PropType::TREE_PINE;
        m_props.push_back(propL1);

        // Outer Forest Row Left (deeper into nature)
        WorldProp propL2;
        float xOffsetL2 = halfRoad + 130.0f + (rand() % 120);
        propL2.pos = Vec2(-xOffsetL2, y + ((rand() % 40) - 20.0f));
        propL2.scale = 1.0f + ((rand() % 40) / 100.0f);

        int rTypeL2 = rand() % 8;
        if (rTypeL2 < 3) propL2.type = PropType::TREE_OAK_LARGE;
        else if (rTypeL2 < 5) propL2.type = PropType::TREE_PINE;
        else if (rTypeL2 < 7) propL2.type = PropType::TREE_CHERRY_BLOSSOM;
        else propL2.type = PropType::BUSH_GREEN;
        m_props.push_back(propL2);

        // --- RIGHT SIDE NATURE (Inner row, Mid row, Outer forest) ---
        // Inner Row Right
        WorldProp propR1;
        float xOffsetR1 = halfRoad + 45.0f + (rand() % 35);
        propR1.pos = Vec2(xOffsetR1, y + ((rand() % 30) - 15.0f));
        propR1.scale = 0.85f + ((rand() % 35) / 100.0f);

        int rTypeR1 = rand() % 10;
        if (rTypeR1 < 5) propR1.type = PropType::TREE_OAK_LARGE;
        else if (rTypeR1 < 7) propR1.type = PropType::TREE_OAK_MEDIUM;
        else if (rTypeR1 < 8) propR1.type = PropType::TREE_CHERRY_BLOSSOM;
        else if (rTypeR1 < 9) propR1.type = PropType::BUSH_FLOWER;
        else propR1.type = PropType::TREE_PINE;
        m_props.push_back(propR1);

        // Outer Forest Row Right
        WorldProp propR2;
        float xOffsetR2 = halfRoad + 130.0f + (rand() % 120);
        propR2.pos = Vec2(xOffsetR2, y + ((rand() % 40) - 20.0f));
        propR2.scale = 1.0f + ((rand() % 40) / 100.0f);

        int rTypeR2 = rand() % 8;
        if (rTypeR2 < 3) propR2.type = PropType::TREE_OAK_LARGE;
        else if (rTypeR2 < 5) propR2.type = PropType::TREE_PINE;
        else if (rTypeR2 < 7) propR2.type = PropType::TREE_CHERRY_BLOSSOM;
        else propR2.type = PropType::BUSH_GREEN;
        m_props.push_back(propR2);

        // Roadside Boulders and Flower patches occasionally
        if (rand() % 4 == 0) {
            WorldProp bush;
            bool side = (rand() % 2 == 0);
            float bx = (side ? -1.0f : 1.0f) * (halfRoad + 25.0f + (rand() % 30));
            bush.pos = Vec2(bx, y + 25.0f);
            bush.scale = 0.7f;
            bush.type = (rand() % 2 == 0) ? PropType::BUSH_FLOWER : PropType::ROCK_BOULDER;
            m_props.push_back(bush);
        }

        // On-road Pickups (Coins, Nitro Tanks) in lanes
        if (rand() % 6 == 0) {
            WorldProp pickup;
            int lane = (rand() % 4) - 2; // -2, -1, 0, 1
            float laneX = (lane + 0.5f) * (m_roadWidth / 4.0f);
            pickup.pos = Vec2(laneX, y + (rand() % 40));
            pickup.width = 24.0f;
            pickup.height = 24.0f;
            pickup.type = (rand() % 4 == 0) ? PropType::NITRO_PICKUP : PropType::COIN_PICKUP;
            m_props.push_back(pickup);
        }
    }
}
