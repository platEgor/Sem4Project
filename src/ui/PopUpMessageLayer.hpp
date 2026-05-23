#pragma once

#include <SFML/Graphics.hpp>

#include "ui/UiTheme.hpp"

#include <string>

class PopupMessageLayer {
public:
    explicit PopupMessageLayer(const sf::Font& font);

    void applyTheme(const UiTheme& theme);

    void show(const std::string& message,
              sf::Color color = sf::Color::White,
              float durationSeconds = 2.5f);

    void update(float dt);

    void draw(sf::RenderWindow& window,
              float screenWidth,
              float uiScale);

private:
    sf::Text messageText;
    UiTheme theme;
    std::string currentMessage;
    float remainingSeconds = 0.f;
};