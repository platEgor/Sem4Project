#pragma once

#include <SFML/Graphics.hpp>

#include "core/Vec2.hpp"
#include "render/Camera.hpp"

#include <cstddef>
#include <optional>
#include <vector>

struct ApsisPoints {
    Vec2 apoapsis;
    Vec2 periapsis;
};

class OrbitTrail {
public:
    OrbitTrail(std::size_t maxPoints,
               float minPointDistance,
               const Vec2& orbitCenter);

    void reset(const Vec2& firstPoint);

    void clear();

    void addIfFarEnough(const Vec2& point);

    void draw(sf::RenderWindow& window,
              const Camera& camera);

    std::optional<ApsisPoints> findApsisPoints(const Vec2& center) const;

private:
    template <typename Callback>
    void forEachStoredPointIndex(Callback callback) const {
        if (filled < points.size()) {
            for (std::size_t i = 0; i < filled; ++i) {
                callback(i);
            }
            return;
        }

        for (std::size_t i = nextIndex; i < points.size(); ++i) {
            callback(i);
        }

        for (std::size_t i = 0; i < nextIndex; ++i) {
            callback(i);
        }
    }

    void addPoint(const Vec2& point);

    void updateOrbitCompletion(const Vec2& point);

private:
    std::vector<Vec2> points;
    std::vector<sf::Vertex> screenPoints;

    float minPointDistance;
    Vec2 orbitCenter;

    float accumulatedAngle = 0.f;
    std::size_t nextIndex = 0;
    std::size_t filled = 0;

    Vec2 lastPoint{0.f, 0.f};

    bool hasLastPoint = false;
    bool completedFullOrbit = false;
};