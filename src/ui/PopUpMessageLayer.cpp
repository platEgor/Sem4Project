#include "ui/PopupMessageLayer.hpp"

#include <algorithm>

PopupMessageLayer::PopupMessageLayer(const sf::Font& font)
    : messageText(font) {}

void PopupMessageLayer::applyTheme(const UiTheme& theme) {
    this->theme = theme;

    messageText.setCharacterSize(theme.fontSize);
    messageText.setFillColor(sf::Color::White);
}

void PopupMessageLayer::show(const std::string& message,
                             sf::Color color,
                             float durationSeconds) {
    currentMessage = message;
    remainingSeconds = durationSeconds;

    messageText.setString(currentMessage);
    messageText.setFillColor(color);
}

void PopupMessageLayer::update(float dt) {
    if (remainingSeconds > 0.f) {
        remainingSeconds = std::max(0.f, remainingSeconds - dt);
    }
}

void PopupMessageLayer::draw(sf::RenderWindow& window,
                             float screenWidth,
                             float uiScale) {
    if (remainingSeconds <= 0.f || currentMessage.empty()) {
        return;
    }

    const float approximateWidth =
        static_cast<float>(currentMessage.size()) *
        static_cast<float>(theme.fontSize) *
        0.55f;

    messageText.setPosition(sf::Vector2f(
        screenWidth / 2.f - approximateWidth / 2.f,
        40.f * uiScale
    ));

    window.draw(messageText);
}