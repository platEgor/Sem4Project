#pragma once

#include "core/Vec2.hpp"

inline constexpr float piValue = 3.14159265358979323846f;
inline constexpr float twoPiValue = 2.f * piValue;

float normalizeAnglePositive(float angle);
float degreesToRadians(float degrees);
float radiansToDegrees(float radians);

float dotProduct(const Vec2& a, const Vec2& b);
float crossProduct(const Vec2& a, const Vec2& b);
Vec2 normalizeOrFallback(const Vec2& value, const Vec2& fallback);
Vec2 perpendicularWithOrientation(const Vec2& direction, float angularMomentum);
float signedAngleBetween(const Vec2& from, const Vec2& to);