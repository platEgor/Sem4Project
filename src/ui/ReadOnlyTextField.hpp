#pragma once

#include "ui/LabeledField.hpp"

#include <functional>
#include <string>
#include <utility>

class ReadOnlyTextField : public LabeledField {
public:
    using Getter = std::function<std::string()>;

    ReadOnlyTextField(const sf::Font& font,
                      const std::string& label,
                      Getter getter)
        : LabeledField(font, label),
          getter(std::move(getter)) {
        refreshValueText();
    }

    void setState(FieldState) override {
        labelText.setFillColor(theme.normalColor);
        valueText.setFillColor(theme.normalColor);
    }

    void refresh() override {
        refreshValueText();
    }

    bool isSelectable() const override {
        return false;
    }

private:
    void refreshValueText() {
        if (getter) {
            valueText.setString(getter());
        }
    }

private:
    Getter getter;
};