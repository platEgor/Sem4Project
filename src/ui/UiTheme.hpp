#pragma once

#include <SFML/Graphics/Color.hpp>

#include "core/AppTypes.hpp"

struct UiTheme {
    sf::Color normalColor = sf::Color::White;
    sf::Color selectedColor = sf::Color::Yellow;
    sf::Color editingColor = sf::Color::Green;
    sf::Color valueColor = sf::Color::Cyan;
    sf::Color helpColor = sf::Color(200, 200, 200);

    unsigned int fontSize = 40;
    unsigned int titleSize = 52;
    unsigned int helpSize = 32;

    float marginX = 40.f;
    float marginY = 30.f;
    float lineGap = 68.f;
    float valueX = 360.f;
};

UiTheme makeTheme(float uiScale);

sf::Color modeToColor(AppMode mode);
sf::Color labelColorFor(FieldState state, const UiTheme& theme);
sf::Color valueColorFor(FieldState state, const UiTheme& theme);