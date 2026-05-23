#include "ui/UiTheme.hpp"

UiTheme makeTheme(float uiScale) {
    UiTheme theme;

    theme.fontSize = static_cast<unsigned int>(20.f * uiScale);
    theme.titleSize = static_cast<unsigned int>(26.f * uiScale);
    theme.helpSize = static_cast<unsigned int>(16.f * uiScale);

    theme.marginX = 20.f * uiScale;
    theme.marginY = 15.f * uiScale;
    theme.lineGap = 34.f * uiScale;
    theme.valueX = 180.f * uiScale;

    return theme;
}

sf::Color modeToColor(AppMode mode) {
    switch (mode) {
        case AppMode::Setup:   return sf::Color::Cyan;
        case AppMode::Running: return sf::Color::Green;
        case AppMode::Paused:  return sf::Color::Yellow;
    }

    return sf::Color::White;
}

sf::Color labelColorFor(FieldState state, const UiTheme& theme) {
    switch (state) {
        case FieldState::Normal:   return theme.normalColor;
        case FieldState::Selected: return theme.selectedColor;
        case FieldState::Editing:  return theme.selectedColor;
    }

    return theme.normalColor;
}

sf::Color valueColorFor(FieldState state, const UiTheme& theme) {
    switch (state) {
        case FieldState::Normal:   return theme.valueColor;
        case FieldState::Selected: return theme.selectedColor;
        case FieldState::Editing:  return theme.editingColor;
    }

    return theme.valueColor;
}