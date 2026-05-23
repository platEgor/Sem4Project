#pragma once

#include "core/AppTypes.hpp"
#include "core/Vec2.hpp"
#include "physics/OrbitMath.hpp"

#include <vector>

std::vector<Vec2> buildEllipseTrajectory(const KeplerianOrbit2D& orbit);

std::vector<Vec2> predictSetupTrajectory(
    const State& initialState,
    const Vec2& center,
    float gravitationalConstant,
    float centralMass,
    float centralBodyRadius
);