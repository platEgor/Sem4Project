#pragma once

#include "ui/LabeledField.hpp"

#include <filesystem>
#include <functional>
#include <string>
#include <utility>
#include <vector>

class FileChoiceField : public LabeledField {
public:
    using OptionsGetter = std::function<std::vector<std::filesystem::path>()>;
    using SelectCallback = std::function<void(const std::filesystem::path&)>;

    FileChoiceField(const sf::Font& font,
                    const std::string& label,
                    OptionsGetter getOptions,
                    SelectCallback onSelected)
        : LabeledField(font, label),
          getOptions(std::move(getOptions)),
          onSelected(std::move(onSelected)) {
        reloadOptions(false);
        refreshValueText();
    }

    void onLeft() override {
        moveSelection(-1);
    }

    void onRight() override {
        moveSelection(1);
    }

    void onEnter() override {
        reloadOptions(true);
        loadSelected();
    }

    void refresh() override {
        reloadOptions(true);
        refreshValueText();
    }

private:
    void reloadOptions(bool preserveCurrentSelection) {
        std::string currentFileName;

        if (preserveCurrentSelection && selectedIndex < options.size()) {
            currentFileName = options[selectedIndex]
                                  .filename()
                                  .generic_string();
        }

        options = getOptions ? getOptions()
                             : std::vector<std::filesystem::path>{};

        if (options.empty()) {
            selectedIndex = 0;
            return;
        }

        if (!currentFileName.empty()) {
            for (std::size_t i = 0; i < options.size(); ++i) {
                if (options[i].filename().generic_string() == currentFileName) {
                    selectedIndex = i;
                    return;
                }
            }
        }

        if (selectedIndex >= options.size()) {
            selectedIndex = 0;
        }
    }

    void moveSelection(int direction) {
        reloadOptions(true);

        if (options.empty()) {
            refreshValueText();
            return;
        }

        if (options.size() == 1) {
            selectedIndex = 0;
        } else if (direction < 0) {
            selectedIndex = (selectedIndex + options.size() - 1) %
                            options.size();
        } else {
            selectedIndex = (selectedIndex + 1) % options.size();
        }

        refreshValueText();
        loadSelected();
    }

    void loadSelected() {
        if (!options.empty() &&
            selectedIndex < options.size() &&
            onSelected) {
            onSelected(options[selectedIndex]);
        }
    }

    void refreshValueText() {
        valueText.setString(
            options.empty()
                ? "<none>"
                : options[selectedIndex].filename().generic_string()
        );
    }

private:
    OptionsGetter getOptions;
    SelectCallback onSelected;

    std::vector<std::filesystem::path> options;
    std::size_t selectedIndex = 0;
};