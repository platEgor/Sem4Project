#pragma once

#include <SFML/Graphics.hpp>

#include "render/Camera.hpp"
#include "render/OrbitTrail.hpp"
#include "ui/UiTheme.hpp"

class ApsisMarkerRenderer {
public:
    explicit ApsisMarkerRenderer(const sf::Font& font);

    void applyTheme(const UiTheme& theme);

    void draw(sf::RenderWindow& window,
              const ApsisPoints& apsisPoints,
              const Camera& camera,
              float uiScale);

private:
    void drawMarker(sf::RenderWindow& window,
                    const Vec2& worldPosition,
                    const Camera& camera,
                    float uiScale,
                    bool anchorAtBottomCenter,
                    bool hasIcon,
                    const sf::Texture& texture,
                    sf::Sprite& sprite,
                    sf::Text& fallbackLabel);

private:
    sf::Texture apoapsisTexture;
    sf::Texture periapsisTexture;

    sf::Sprite apoapsisSprite;
    sf::Sprite periapsisSprite;

    sf::Text apoapsisFallbackLabel;
    sf::Text periapsisFallbackLabel;

    UiTheme theme;

    bool hasApoapsisIcon = false;
    bool hasPeriapsisIcon = false;
};