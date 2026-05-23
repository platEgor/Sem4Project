#pragma once

#include <SFML/Graphics.hpp>

#include "ui/SettingsField.hpp"
#include "ui/UiTheme.hpp"

#include <memory>
#include <string>
#include <utility>
#include <vector>

enum class HorizontalAnchor {
    Left,
    Right
};

class SetupScreen {
public:
    explicit SetupScreen(const sf::Font& font,
                         const std::string& titleText = "Setup Menu");

    void setBaseLayout(float titleX,
                       float titleY,
                       float fieldLabelX,
                       float firstFieldY,
                       float fieldValueX);

    void setRightAnchoredLayout(float screenWidth,
                                float rightMargin,
                                float titleY,
                                float panelWidth,
                                float firstFieldY,
                                float valueOffset);

    void addField(std::unique_ptr<SettingsField> field);

    void applyTheme(const UiTheme& theme);

    void handleKey(sf::Keyboard::Key key);

    void handleTextEntered(char ch);

    bool isEditing() const;

    void refresh();

    void draw(sf::RenderWindow& window) const;

private:
    float scale() const;

    bool hasSelectableField() const;

    void ensureSelectableIndex();

    void moveSelection(int direction);

    void updateSelectionStates();

private:
    sf::Text title;
    UiTheme theme;
    std::vector<std::unique_ptr<SettingsField>> fields;
    std::size_t selectedIndex = 0;

    HorizontalAnchor anchor = HorizontalAnchor::Left;

    float titleXBase = 20.f;
    float titleYBase = 70.f;
    float fieldLabelXBase = 20.f;
    float firstFieldYBase = 130.f;
    float fieldValueXBase = 180.f;

    float screenWidthForRightAnchor = 1920.f;
    float rightMarginBase = 20.f;
    float panelWidthBase = 360.f;
    float valueOffsetBase = 170.f;
};

template <typename Field, typename... Args>
void addField(SetupScreen& screen, Args&&... args) {
    screen.addField(std::make_unique<Field>(std::forward<Args>(args)...));
}