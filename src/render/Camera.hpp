#pragma once

#include <SFML/System/Vector2.hpp>

#include "core/Vec2.hpp"

struct Camera {
    Vec2 center{0.f, 0.f};
    float zoom = 1.f;
    float screenWidth = 1920.f;
    float screenHeight = 1080.f;

    sf::Vector2f toScreen(const Vec2& world) const {
        return {
            (world.x - center.x) * zoom + screenWidth / 2.f,
            (world.y - center.y) * zoom + screenHeight / 2.f
        };
    }

    Vec2 toWorld(sf::Vector2f screen) const {
        return {
            center.x + (screen.x - screenWidth / 2.f) / zoom,
            center.y + (screen.y - screenHeight / 2.f) / zoom
        };
    }
};