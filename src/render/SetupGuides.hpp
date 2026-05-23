#pragma once

#include <SFML/Graphics.hpp>

#include "core/AppTypes.hpp"
#include "core/Vec2.hpp"
#include "render/Camera.hpp"
#include "ui/UiTheme.hpp"

void drawCartesianSetupGuides(sf::RenderWindow& window,
                              const sf::Font& font,
                              const UiTheme& theme,
                              const State& initialState,
                              const Vec2& origin,
                              const Camera& camera,
                              float uiScale);

void drawKeplerianSetupGuides(sf::RenderWindow& window,
                              const sf::Font& font,
                              const UiTheme& theme,
                              const State& initialState,
                              const Vec2& focus,
                              float gravitationalParameter,
                              float centralBodyRadius,
                              const Camera& camera,
                              float uiScale);