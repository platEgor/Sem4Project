#pragma once

#include <SFML/Graphics.hpp>

#include "core/AppTypes.hpp"
#include "ui/UiTheme.hpp"

class SettingsField {
public:
    virtual ~SettingsField() = default;

    virtual void applyTheme(const UiTheme& theme) = 0;
    virtual void setPosition(sf::Vector2f labelPosition,
                             sf::Vector2f valuePosition) = 0;
    virtual void setState(FieldState state) = 0;
    virtual void draw(sf::RenderWindow& window) const = 0;

    virtual void onEnter() {}
    virtual void onEscape() {}
    virtual void onBackspace() {}
    virtual void onTextEntered(char) {}
    virtual void onLeft() {}
    virtual void onRight() {}
    virtual void refresh() {}

    virtual bool isEditing() const {
        return false;
    }

    virtual bool isSelectable() const {
        return true;
    }

    virtual bool isVisible() const {
        return true;
    }
};