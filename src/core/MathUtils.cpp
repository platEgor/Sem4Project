#include "core/MathUtils.hpp"

#include <cmath>

float normalizeAnglePositive(float angle) {
    float result = std::fmod(angle, twoPiValue);

    if (result < 0.f) {
        result += twoPiValue;
    }

    return result;
}

float degreesToRadians(float degrees) {
    return degrees * piValue / 180.f;
}

float radiansToDegrees(float radians) {
    return radians * 180.f / piValue;
}

float dotProduct(const Vec2& a, const Vec2& b) {
    return a.x * b.x + a.y * b.y;
}

float crossProduct(const Vec2& a, const Vec2& b) {
    return a.x * b.y - a.y * b.x;
}

Vec2 normalizeOrFallback(const Vec2& value, const Vec2& fallback) {
    const float length = value.length();

    if (length <= 1e-5f) {
        return fallback;
    }

    return value / length;
}

Vec2 perpendicularWithOrientation(const Vec2& direction, float angularMomentum) {
    if (angularMomentum >= 0.f) {
        return Vec2{-direction.y, direction.x};
    }

    return Vec2{direction.y, -direction.x};
}

float signedAngleBetween(const Vec2& from, const Vec2& to) {
    const float cross = from.x * to.y - from.y * to.x;
    const float dot = from.x * to.x + from.y * to.y;

    return std::atan2(cross, dot);
}