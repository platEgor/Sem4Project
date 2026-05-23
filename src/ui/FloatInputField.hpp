#pragma once

#include "core/Format.hpp"
#include "ui/LabeledField.hpp"

#include <algorithm>
#include <functional>
#include <string>
#include <utility>

class FloatInputField : public LabeledField {
public:
    using Getter = std::function<float()>;
    using Setter = std::function<void(float)>;
    using ChangeCallback = std::function<void()>;
    using VisibilityPredicate = std::function<bool()>;

    FloatInputField(const sf::Font& font,
                    const std::string& label,
                    Getter getter,
                    Setter setter,
                    float minValue,
                    float maxValue,
                    int digits,
                    ChangeCallback onChanged = {},
                    VisibilityPredicate isVisiblePredicate = {})
        : LabeledField(font, label),
          getter(std::move(getter)),
          setter(std::move(setter)),
          onChanged(std::move(onChanged)),
          isVisiblePredicate(std::move(isVisiblePredicate)),
          minValue(minValue),
          maxValue(maxValue),
          digits(digits) {
        refreshValueText();
    }

    void onEnter() override {
        if (!editing) {
            editing = true;
            buffer = formatFloat(getter(), digits);
            valueText.setString(buffer);
            setState(FieldState::Editing);
            return;
        }

        try {
            float value = std::stof(buffer);
            setter(std::clamp(value, minValue, maxValue));

            if (onChanged) {
                onChanged();
            }
        } catch (...) {
            // If input is invalid, keep the previous value.
        }

        editing = false;
        buffer.clear();

        refreshValueText();
        setState(FieldState::Selected);
    }

    void onEscape() override {
        editing = false;
        buffer.clear();

        refreshValueText();
        setState(FieldState::Selected);
    }

    void onBackspace() override {
        if (editing && !buffer.empty()) {
            buffer.pop_back();
            valueText.setString(buffer);
        }
    }

    void onTextEntered(char ch) override {
        if (!editing) {
            return;
        }

        const bool isDigit = (ch >= '0' && ch <= '9');
        const bool isDot = (ch == '.');
        const bool isMinus = (ch == '-');

        if (isDigit) {
            buffer.push_back(ch);
        } else if (isDot && buffer.find('.') == std::string::npos) {
            buffer.push_back(ch);
        } else if (isMinus && buffer.empty() && minValue < 0.f) {
            buffer.push_back(ch);
        }

        valueText.setString(buffer);
    }

    bool isEditing() const override {
        return editing;
    }

    bool isVisible() const override {
        return !isVisiblePredicate || isVisiblePredicate();
    }

    void refresh() override {
        if (!editing) {
            refreshValueText();
        }
    }

private:
    void refreshValueText() {
        valueText.setString(formatFloat(getter(), digits));
    }

private:
    Getter getter;
    Setter setter;
    ChangeCallback onChanged;
    VisibilityPredicate isVisiblePredicate;

    std::string buffer;

    float minValue;
    float maxValue;
    int digits;

    bool editing = false;
};