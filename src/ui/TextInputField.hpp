#pragma once

#include "core/Format.hpp"
#include "ui/LabeledField.hpp"

#include <functional>
#include <string>
#include <utility>

class TextInputField : public LabeledField {
public:
    using SubmitCallback = std::function<void(const std::string&)>;

    TextInputField(const sf::Font& font,
                   const std::string& label,
                   const std::string& placeholder,
                   SubmitCallback onSubmit)
        : LabeledField(font, label),
          placeholder(placeholder),
          onSubmit(std::move(onSubmit)) {
        refreshValueText();
    }

    void onEnter() override {
        if (!editing) {
            editing = true;
            buffer = value;
            valueText.setString(buffer);
            setState(FieldState::Editing);
            return;
        }

        value = trimCopy(buffer);

        if (onSubmit) {
            onSubmit(value);
        }

        value.clear();
        buffer.clear();
        editing = false;

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

        const unsigned char code = static_cast<unsigned char>(ch);

        if (code >= 32 && code <= 126) {
            buffer.push_back(ch);
            valueText.setString(buffer);
        }
    }

    bool isEditing() const override {
        return editing;
    }

    void refresh() override {
        if (!editing) {
            refreshValueText();
        }
    }

private:
    void refreshValueText() {
        valueText.setString(value.empty() ? placeholder : value);
    }

private:
    std::string placeholder;
    std::string value;
    std::string buffer;

    SubmitCallback onSubmit;

    bool editing = false;
};