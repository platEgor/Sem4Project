#include "render/OrbitTrail.hpp"

#include "core/MathUtils.hpp"
#include "render/DrawPrimitives.hpp"

#include <cmath>
#include <limits>

OrbitTrail::OrbitTrail(std::size_t maxPoints,
                       float minPointDistance,
                       const Vec2& orbitCenter)
    : points(maxPoints),
      screenPoints(maxPoints),
      minPointDistance(minPointDistance),
      orbitCenter(orbitCenter) {}

void OrbitTrail::reset(const Vec2& firstPoint) {
    nextIndex = 0;
    filled = 0;
    hasLastPoint = false;
    accumulatedAngle = 0.f;
    completedFullOrbit = false;

    addPoint(firstPoint);
}

void OrbitTrail::clear() {
    nextIndex = 0;
    filled = 0;
    hasLastPoint = false;
    accumulatedAngle = 0.f;
    completedFullOrbit = false;
}

void OrbitTrail::addIfFarEnough(const Vec2& point) {
    if (!hasLastPoint || (point - lastPoint).length() >= minPointDistance) {
        addPoint(point);
    }
}

void OrbitTrail::draw(sf::RenderWindow& window,
                      const Camera& camera) {
    if (filled < 2 || points.empty()) {
        return;
    }

    std::size_t screenCount = 0;

    auto appendPoint = [&](std::size_t pointIndex) {
        screenPoints[screenCount].position = worldToScreen(
            points[pointIndex],
            camera
        );

        screenPoints[screenCount].color = sf::Color(255, 220, 120);
        ++screenCount;
    };

    forEachStoredPointIndex(appendPoint);

    if (screenCount > 1) {
        window.draw(
            screenPoints.data(),
            static_cast<int>(screenCount),
            sf::PrimitiveType::LineStrip
        );
    }
}

std::optional<ApsisPoints> OrbitTrail::findApsisPoints(const Vec2& center) const {
    if (!completedFullOrbit || filled < 2 || points.empty()) {
        return std::nullopt;
    }

    float minRadius = std::numeric_limits<float>::max();
    float maxRadius = 0.f;

    Vec2 periapsis{0.f, 0.f};
    Vec2 apoapsis{0.f, 0.f};

    auto inspectPoint = [&](std::size_t pointIndex) {
        const Vec2 point = points[pointIndex];
        const float radius = (point - center).length();

        if (radius < minRadius) {
            minRadius = radius;
            periapsis = point;
        }

        if (radius > maxRadius) {
            maxRadius = radius;
            apoapsis = point;
        }
    };

    forEachStoredPointIndex(inspectPoint);

    return ApsisPoints{apoapsis, periapsis};
}

void OrbitTrail::addPoint(const Vec2& point) {
    if (points.empty()) {
        return;
    }

    updateOrbitCompletion(point);

    points[nextIndex] = point;
    nextIndex = (nextIndex + 1) % points.size();

    if (filled < points.size()) {
        ++filled;
    }

    lastPoint = point;
    hasLastPoint = true;
}

void OrbitTrail::updateOrbitCompletion(const Vec2& point) {
    if (!hasLastPoint) {
        return;
    }

    const Vec2 previousRelative = lastPoint - orbitCenter;
    const Vec2 currentRelative = point - orbitCenter;

    if (previousRelative.length() <= 1e-5f ||
        currentRelative.length() <= 1e-5f) {
        return;
    }

    accumulatedAngle += signedAngleBetween(previousRelative, currentRelative);

    constexpr float fullOrbitRadians = 2.f * 3.14159265358979323846f;

    if (std::abs(accumulatedAngle) >= fullOrbitRadians) {
        completedFullOrbit = true;
    }
}