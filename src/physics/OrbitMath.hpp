#pragma once

#include "core/AppTypes.hpp"
#include "core/Vec2.hpp"

#include <optional>

struct KeplerianOrbit2D {
    Vec2 focus{0.f, 0.f};
    Vec2 center{0.f, 0.f};
    Vec2 periapsisDirection{1.f, 0.f};
    Vec2 minorAxisDirection{0.f, 1.f};
    Vec2 eccentricityVector{0.f, 0.f};

    float semiMajorAxis = 0.f;
    float semiMinorAxis = 0.f;
    float eccentricity = 0.f;
    float specificEnergy = 0.f;
    float specificAngularMomentum = 0.f;
    float argumentOfPeriapsis = 0.f;
    float trueAnomaly = 0.f;

    bool isBoundEllipse = false;
};

std::optional<KeplerianOrbit2D> cartesianToKeplerianOrbit(
    const State& state,
    const Vec2& center,
    float gravitationalParameter
);

std::optional<State> keplerianToCartesianState(
    const KeplerianInitialConditions& elements,
    const Vec2& center,
    float gravitationalParameter
);