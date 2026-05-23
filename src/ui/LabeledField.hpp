#pragma once

#include <SFML/Graphics.hpp>

#include "ui/SettingsField.hpp"
#include "ui/UiTheme.hpp"

#include <cmath>
#include <string>
#include <utility>

class LabeledField : public SettingsField {
public:
    LabeledField(const sf::Font& font,
                 std::string label,
                 float textScale = 1.f)
        : labelText(font),
          valueText(font),
          label(std::move(label)),
          textScale(textScale) {
        labelText.setString(this->label);
    }

    void applyTheme(const UiTheme& theme) override {
        this->theme = theme;

        const unsigned int size = static_cast<unsigned int>(
            std::round(static_cast<float>(theme.fontSize) * textScale)
        );

        labelText.setCharacterSize(size);
        valueText.setCharacterSize(size);

        setState(state);
    }

    void setPosition(sf::Vector2f labelPosition,
                     sf::Vector2f valuePosition) override {
        labelText.setPosition(labelPosition);
        valueText.setPosition(valuePosition);
    }

    void setState(FieldState newState) override {
        state = newState;

        labelText.setFillColor(labelColorFor(state, theme));
        valueText.setFillColor(valueColorFor(state, theme));
    }

    void draw(sf::RenderWindow& window) const override {
        window.draw(labelText);
        window.draw(valueText);
    }

protected:
    sf::Text labelText;
    sf::Text valueText;

    std::string label;

    UiTheme theme;
    FieldState state = FieldState::Normal;

    float textScale = 1.f;
};