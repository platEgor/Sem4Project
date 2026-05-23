#pragma once

#include <cmath>

struct Vec2 {
    float x = 0.f;
    float y = 0.f;

    float length() const {
        return std::sqrt(x * x + y * y);
    }

    Vec2 operator+(const Vec2& other) const {
        return {x + other.x, y + other.y};
    }

    Vec2 operator-(const Vec2& other) const {
        return {x - other.x, y - other.y};
    }

    Vec2 operator*(float k) const {
        return {x * k, y * k};
    }

    Vec2 operator/(float k) const {
        return {x / k, y / k};
    }

    Vec2& operator+=(const Vec2& other) {
        x += other.x;
        y += other.y;
        return *this;
    }
};