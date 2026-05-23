#include "render/ApsisMarkerRenderer.hpp"

#include "render/DrawPrimitives.hpp"

#include <algorithm>

ApsisMarkerRenderer::ApsisMarkerRenderer(const sf::Font& font)
    : apoapsisSprite(apoapsisTexture),
      periapsisSprite(periapsisTexture),
      apoapsisFallbackLabel(font),
      periapsisFallbackLabel(font) {
    hasApoapsisIcon =
        apoapsisTexture.loadFromFile("assets/textures/apoapsis.png");

    hasPeriapsisIcon =
        periapsisTexture.loadFromFile("assets/textures/periapsis.png");

    if (hasApoapsisIcon) {
        apoapsisSprite.setTexture(apoapsisTexture, true);
    }

    if (hasPeriapsisIcon) {
        periapsisSprite.setTexture(periapsisTexture, true);
    }

    apoapsisFallbackLabel.setString("Apocenter");
    periapsisFallbackLabel.setString("Pericenter");
}

void ApsisMarkerRenderer::applyTheme(const UiTheme& theme) {
    this->theme = theme;

    apoapsisFallbackLabel.setCharacterSize(theme.helpSize);
    periapsisFallbackLabel.setCharacterSize(theme.helpSize);

    apoapsisFallbackLabel.setFillColor(theme.normalColor);
    periapsisFallbackLabel.setFillColor(theme.normalColor);
}

void ApsisMarkerRenderer::draw(sf::RenderWindow& window,
                               const ApsisPoints& apsisPoints,
                               const Camera& camera,
                               float uiScale) {
    drawMarker(
        window,
        apsisPoints.apoapsis,
        camera,
        uiScale,
        true,
        hasApoapsisIcon,
        apoapsisTexture,
        apoapsisSprite,
        apoapsisFallbackLabel
    );

    drawMarker(
        window,
        apsisPoints.periapsis,
        camera,
        uiScale,
        false,
        hasPeriapsisIcon,
        periapsisTexture,
        periapsisSprite,
        periapsisFallbackLabel
    );
}

void ApsisMarkerRenderer::drawMarker(sf::RenderWindow& window,
                                     const Vec2& worldPosition,
                                     const Camera& camera,
                                     float uiScale,
                                     bool anchorAtBottomCenter,
                                     bool hasIcon,
                                     const sf::Texture& texture,
                                     sf::Sprite& sprite,
                                     sf::Text& fallbackLabel) {
    const sf::Vector2f screenPosition = worldToScreen(
        worldPosition,
        camera
    );

    if (hasIcon) {
        const sf::Vector2u textureSize = texture.getSize();
        const float textureWidth = static_cast<float>(textureSize.x);
        const float textureHeight = static_cast<float>(textureSize.y);
        const float maxTextureSide = std::max(textureWidth, textureHeight);

        if (maxTextureSide > 0.f) {
            const float targetScreenSize = 32.f * uiScale;
            const float iconScale = targetScreenSize / maxTextureSide;

            if (anchorAtBottomCenter) {
                sprite.setOrigin(sf::Vector2f(
                    textureWidth / 2.f,
                    textureHeight
                ));
            } else {
                sprite.setOrigin(sf::Vector2f(
                    textureWidth / 2.f,
                    0.f
                ));
            }

            sprite.setScale(sf::Vector2f(iconScale, iconScale));
            sprite.setPosition(screenPosition);

            window.draw(sprite);
            return;
        }
    }

    const float verticalOffset =
        static_cast<float>(theme.helpSize) / 2.f;

    const float horizontalOffset = 8.f * uiScale;

    if (anchorAtBottomCenter) {
        fallbackLabel.setPosition(sf::Vector2f(
            screenPosition.x + horizontalOffset,
            screenPosition.y - 2.f * verticalOffset
        ));
    } else {
        fallbackLabel.setPosition(sf::Vector2f(
            screenPosition.x + horizontalOffset,
            screenPosition.y + verticalOffset
        ));
    }

    window.draw(fallbackLabel);
}