#pragma once

#include "ui/LabeledField.hpp"

#include <functional>
#include <string>
#include <utility>

class ButtonField : public LabeledField {
public:
    using Action = std::function<void()>;

    ButtonField(const sf::Font& font,
                const std::string& label,
                Action action)
        : LabeledField(font, label, 1.25f),
          action(std::move(action)) {}

    void setPosition(sf::Vector2f labelPosition,
                     sf::Vector2f) override {
        labelText.setPosition(labelPosition);
    }

    void draw(sf::RenderWindow& window) const override {
        window.draw(labelText);
    }

    void onEnter() override {
        if (action) {
            action();
        }
    }

private:
    Action action;
};