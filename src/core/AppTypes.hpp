#pragma once

#include "core/Vec2.hpp"

struct State {
    Vec2 r;
    Vec2 v;
    Vec2 a;
};

enum class AppMode {
    Setup,
    Running,
    Paused
};

enum class InitialConditionMode {
    Cartesian,
    Keplerian
};

enum class FieldState {
    Normal,
    Selected,
    Editing
};

struct CartesianInitialConditions {
    float x = 0.f;
    float y = 0.f;
    float vx = 0.f;
    float vy = 0.f;
};

struct KeplerianInitialConditions {
    float semiMajorAxis = 0.f;
    float eccentricity = 0.f;
    float argumentOfPeriapsis = 0.f;
    float trueAnomaly = 0.f;
    int motionDirection = 1;
};

struct SimulationSettings {
    float uiScale = 1.5f;
    int timeWarp = 1000;
    InitialConditionMode initialConditionMode = InitialConditionMode::Cartesian;
    CartesianInitialConditions cartesianInitialConditions;
    KeplerianInitialConditions keplerianInitialConditions;
};

inline const char* modeToString(AppMode mode) {
    switch (mode) {
        case AppMode::Setup:
            return "Setup";
        case AppMode::Running:
            return "Running";
        case AppMode::Paused:
            return "Paused";
    }

    return "Unknown";
}

inline const char* initialConditionModeToString(InitialConditionMode mode) {
    switch (mode) {
        case InitialConditionMode::Cartesian:
            return "Cartesian";
        case InitialConditionMode::Keplerian:
            return "Keplerian";
    }

    return "Unknown";
}