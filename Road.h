#pragma once
#ifndef ROAD_H
#define ROAD_H

#include "Types.h"
#include <vector>
#include <string>

struct RoadPoint {
    Vec2 center;
    float width = 640.0f;
    float angle = 0.0f; // Tangent angle
};

class Road {
public:
    Road();
    ~Road();

    void initTrack(int stageIndex, WeatherType& outWeather);
    void initEndlessTrack();

    float getRoadWidth() const { return m_roadWidth; }
    float getTrackLength() const { return m_totalLength; }
    int getStage() const { return m_stage; }
    std::string getStageName() const { return m_stageName; }
    int getCheckpointsPassed() const { return m_checkpointsPassed; }
    void markCheckpointPassed() { m_checkpointsPassed++; }

    // Road curve lookup at distance Y along highway
    float getRoadCenterX(float worldY) const;
    float getRoadAngle(float worldY) const;

    const std::vector<WorldProp>& getProps() const { return m_props; }
    std::vector<WorldProp>& getPropsRef() { return m_props; }

    ColorRGB getAsphaltColor() const { return m_asphaltColor; }
    ColorRGB getShoulderColor() const { return m_shoulderColor; }
    ColorRGB getCurbColor() const { return m_curbColor; }
    ColorRGB getLineColor() const { return m_lineColor; }

private:
    void generateProps(int stage);

    int m_stage = 1;
    std::string m_stageName = "Pacific Coast Speedway";
    float m_roadWidth = 620.0f;
    float m_totalLength = 25000.0f; // Track distance in world units
    int m_checkpointsPassed = 0;

    ColorRGB m_asphaltColor;
    ColorRGB m_shoulderColor;
    ColorRGB m_curbColor;
    ColorRGB m_lineColor;

    std::vector<WorldProp> m_props;
};

#endif // ROAD_H
