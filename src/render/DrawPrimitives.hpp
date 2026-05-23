#pragma once

#include <SFML/Graphics.hpp>

#include "core/AppTypes.hpp"
#include "core/Vec2.hpp"
#include "render/Camera.hpp"
#include "ui/UiTheme.hpp"

#include <string>
#include <vector>

sf::Vector2f worldToScreen(const Vec2& worldPos, const Camera& camera);

Vec2 screenToWorld(sf::Vector2f screenPos, const Camera& camera);

void drawArrow(sf::RenderWindow& window,
               sf::Vector2f from,
               sf::Vector2f to,
               sf::Color color,
               float thickness,
               float headLength,
               float headWidth);

void drawInitialVelocityArrow(sf::RenderWindow& window,
                              const State& initialState,
                              const Camera& camera,
                              float uiScale);

void drawThinLine(sf::RenderWindow& window,
                  sf::Vector2f from,
                  sf::Vector2f to,
                  sf::Color color);

void drawDashedLine(sf::RenderWindow& window,
                    sf::Vector2f from,
                    sf::Vector2f to,
                    sf::Color color,
                    float dashLength,
                    float gapLength);

void drawDashedWorldLine(sf::RenderWindow& window,
                         const Vec2& worldFrom,
                         const Vec2& worldTo,
                         const Camera& camera,
                         sf::Color color,
                         float dashLength,
                         float gapLength);

void drawDashedWorldPolyline(sf::RenderWindow& window,
                             const std::vector<Vec2>& points,
                             const Camera& camera,
                             sf::Color color,
                             float dashLength,
                             float gapLength);

void drawWorldLabel(sf::RenderWindow& window,
                    const sf::Font& font,
                    const UiTheme& theme,
                    const std::string& label,
                    const Vec2& worldPosition,
                    const Camera& camera,
                    sf::Color color,
                    float offsetX,
                    float offsetY);

void drawWorldArc(sf::RenderWindow& window,
                  const Vec2& center,
                  float radius,
                  float startAngle,
                  float deltaAngle,
                  const Camera& camera,
                  sf::Color color);