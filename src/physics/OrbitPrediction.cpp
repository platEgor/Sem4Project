#include "physics/OrbitPrediction.hpp"

#include "core/MathUtils.hpp"

#include <cmath>
#include <optional>
#include <vector>

std::vector<Vec2> buildEllipseTrajectory(const KeplerianOrbit2D& orbit) {
    std::vector<Vec2> trajectory;

    if (!orbit.isBoundEllipse) {
        return trajectory;
    }

    constexpr int ellipseSegments = 720;
    trajectory.reserve(ellipseSegments + 1);

    for (int i = 0; i <= ellipseSegments; ++i) {
        const float angle =
            twoPiValue * static_cast<float>(i) / static_cast<float>(ellipseSegments);

        const Vec2 point =
            orbit.center +
            orbit.periapsisDirection *
                (orbit.semiMajorAxis * std::cos(angle)) +
            orbit.minorAxisDirection *
                (orbit.semiMinorAxis * std::sin(angle));

        trajectory.push_back(point);
    }

    return trajectory;
}

std::vector<Vec2> predictSetupTrajectory(
    const State& initialState,
    const Vec2& center,
    float gravitationalConstant,
    float centralMass,
    float centralBodyRadius
) {
    const Vec2 initialRelative = initialState.r - center;

    if (initialRelative.length() <= centralBodyRadius) {
        return {initialState.r};
    }

    const float gravitationalParameter = gravitationalConstant * centralMass;

    const std::optional<KeplerianOrbit2D> orbit =
        cartesianToKeplerianOrbit(
            initialState,
            center,
            gravitationalParameter
        );

    if (!orbit) {
        return {initialState.r};
    }

    return buildEllipseTrajectory(*orbit);
}