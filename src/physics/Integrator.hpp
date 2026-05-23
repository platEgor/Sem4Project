#pragma once

#include "core/AppTypes.hpp"
#include "core/Vec2.hpp"

void stepSemiImplicitEuler(State& state,
                           float dt,
                           const Vec2& center,
                           float gravitationalConstant,
                           float centralMass) {
    const Vec2 dr = state.r - center;
    const float dist = dr.length();

    if (dist > 1e-5f) {
        const float accelerationFactor =
            -(gravitationalConstant * centralMass) / std::pow(dist, 3);

        state.a = dr * accelerationFactor;
    } else {
        state.a = {0.f, 0.f};
    }

    state.v += state.a * dt;
    state.r += state.v * dt;
}