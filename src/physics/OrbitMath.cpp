#include "physics/OrbitMath.hpp"

#include "core/MathUtils.hpp"

#include <algorithm>
#include <cmath>

std::optional<KeplerianOrbit2D> cartesianToKeplerianOrbit(
    const State& state,
    const Vec2& center,
    float gravitationalParameter
) {
    const Vec2 relativePosition = state.r - center;
    const Vec2 relativeVelocity = state.v;

    const float radius = relativePosition.length();
    if (radius <= 1e-5f || gravitationalParameter <= 0.f) {
        return std::nullopt;
    }

    const float velocitySquared = dotProduct(relativeVelocity, relativeVelocity);
    const float radialVelocityProduct = dotProduct(relativePosition, relativeVelocity);
    const float specificAngularMomentum = crossProduct(relativePosition, relativeVelocity);
    const float specificEnergy = velocitySquared / 2.f - gravitationalParameter / radius;

    if (specificEnergy >= 0.f) {
        return std::nullopt;
    }

    KeplerianOrbit2D orbit;
    orbit.focus = center;
    orbit.specificEnergy = specificEnergy;
    orbit.specificAngularMomentum = specificAngularMomentum;
    orbit.semiMajorAxis = -gravitationalParameter / (2.f * specificEnergy);

    orbit.eccentricityVector =
        (relativePosition * (velocitySquared - gravitationalParameter / radius) -
         relativeVelocity * radialVelocityProduct) / gravitationalParameter;

    orbit.eccentricity = orbit.eccentricityVector.length();

    if (orbit.semiMajorAxis <= 0.f || orbit.eccentricity >= 1.f) {
        return std::nullopt;
    }

    const Vec2 fallbackPeriapsisDirection =
        normalizeOrFallback(relativePosition, Vec2{1.f, 0.f});

    if (orbit.eccentricity > 1e-4f) {
        orbit.periapsisDirection = orbit.eccentricityVector / orbit.eccentricity;
    } else {
        orbit.periapsisDirection = fallbackPeriapsisDirection;
    }

    orbit.minorAxisDirection = perpendicularWithOrientation(
        orbit.periapsisDirection,
        orbit.specificAngularMomentum
    );

    const float oneMinusESquared =
        std::max(0.f, 1.f - orbit.eccentricity * orbit.eccentricity);

    orbit.semiMinorAxis = orbit.semiMajorAxis * std::sqrt(oneMinusESquared);
    orbit.center = orbit.focus - orbit.eccentricityVector * orbit.semiMajorAxis;
    orbit.argumentOfPeriapsis =
        std::atan2(orbit.periapsisDirection.y, orbit.periapsisDirection.x);

    if (orbit.eccentricity > 1e-4f) {
        const float cosTrueAnomaly = std::clamp(
            dotProduct(orbit.eccentricityVector, relativePosition) /
                (orbit.eccentricity * radius),
            -1.f,
            1.f
        );

        const float sinTrueAnomaly = std::clamp(
            orbit.specificAngularMomentum * radialVelocityProduct /
                (orbit.eccentricity * gravitationalParameter * radius),
            -1.f,
            1.f
        );

        orbit.trueAnomaly = std::atan2(sinTrueAnomaly, cosTrueAnomaly);
    } else {
        orbit.trueAnomaly = std::atan2(
            dotProduct(relativePosition, orbit.minorAxisDirection),
            dotProduct(relativePosition, orbit.periapsisDirection)
        );
    }

    orbit.argumentOfPeriapsis = normalizeAnglePositive(orbit.argumentOfPeriapsis);
    orbit.trueAnomaly = normalizeAnglePositive(orbit.trueAnomaly);
    orbit.isBoundEllipse = true;

    return orbit;
}

std::optional<State> keplerianToCartesianState(
    const KeplerianInitialConditions& elements,
    const Vec2& center,
    float gravitationalParameter
) {
    if (gravitationalParameter <= 0.f || elements.semiMajorAxis <= 0.f) {
        return std::nullopt;
    }

    const float eccentricity = std::clamp(elements.eccentricity, 0.f, 0.9999f);
    const float semiMajorAxis = elements.semiMajorAxis;
    const float parameter = semiMajorAxis * (1.f - eccentricity * eccentricity);

    if (parameter <= 1e-5f) {
        return std::nullopt;
    }

    const float argumentOfPeriapsis =
        normalizeAnglePositive(elements.argumentOfPeriapsis);

    const float trueAnomaly =
        normalizeAnglePositive(elements.trueAnomaly);

    const int directionSign = elements.motionDirection >= 0 ? 1 : -1;

    const Vec2 periapsisDirection{
        std::cos(argumentOfPeriapsis),
        std::sin(argumentOfPeriapsis)
    };

    const Vec2 trueAnomalyDirection = directionSign >= 0
        ? Vec2{-periapsisDirection.y, periapsisDirection.x}
        : Vec2{periapsisDirection.y, -periapsisDirection.x};

    const float denominator = 1.f + eccentricity * std::cos(trueAnomaly);

    if (std::abs(denominator) <= 1e-5f) {
        return std::nullopt;
    }

    const float radius = parameter / denominator;

    const Vec2 relativePosition =
        periapsisDirection * (radius * std::cos(trueAnomaly)) +
        trueAnomalyDirection * (radius * std::sin(trueAnomaly));

    const float velocityScale = std::sqrt(gravitationalParameter / parameter);

    const Vec2 relativeVelocity =
        periapsisDirection * (-velocityScale * std::sin(trueAnomaly)) +
        trueAnomalyDirection *
            (velocityScale * (eccentricity + std::cos(trueAnomaly)));

    return State{
        center + relativePosition,
        relativeVelocity,
        {0.f, 0.f}
    };
}