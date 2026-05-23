#include "ui/SetupScreen.hpp"

#include <algorithm>

SetupScreen::SetupScreen(const sf::Font& font,
                         const std::string& titleText)
    : title(font) {
    title.setString(titleText);
}

void SetupScreen::setBaseLayout(float titleX,
                                float titleY,
                                float fieldLabelX,
                                float firstFieldY,
                                float fieldValueX) {
    anchor = HorizontalAnchor::Left;
    titleXBase = titleX;
    titleYBase = titleY;
    fieldLabelXBase = fieldLabelX;
    firstFieldYBase = firstFieldY;
    fieldValueXBase = fieldValueX;

    applyTheme(theme);
}

void SetupScreen::setRightAnchoredLayout(float screenWidth,
                                         float rightMargin,
                                         float titleY,
                                         float panelWidth,
                                         float firstFieldY,
                                         float valueOffset) {
    anchor = HorizontalAnchor::Right;
    screenWidthForRightAnchor = screenWidth;
    rightMarginBase = rightMargin;
    titleYBase = titleY;
    panelWidthBase = panelWidth;
    firstFieldYBase = firstFieldY;
    valueOffsetBase = valueOffset;

    applyTheme(theme);
}

void SetupScreen::addField(std::unique_ptr<SettingsField> field) {
    fields.push_back(std::move(field));
    applyTheme(theme);
    ensureSelectableIndex();
    updateSelectionStates();
}

void SetupScreen::applyTheme(const UiTheme& theme) {
    this->theme = theme;

    title.setCharacterSize(theme.titleSize);
    title.setFillColor(theme.normalColor);

    const float uiScale = scale();
    float titleX = titleXBase * uiScale;
    float fieldLabelX = fieldLabelXBase * uiScale;
    float fieldValueX = fieldValueXBase * uiScale;

    if (anchor == HorizontalAnchor::Right) {
        const float rightEdge =
            screenWidthForRightAnchor - rightMarginBase * uiScale;

        fieldLabelX = rightEdge - panelWidthBase * uiScale;
        fieldValueX = fieldLabelX + valueOffsetBase * uiScale;
        titleX = fieldLabelX;
    }

    title.setPosition(sf::Vector2f(titleX, titleYBase * uiScale));

    std::size_t visibleRow = 0;

    for (const auto& field : fields) {
        field->refresh();
        field->applyTheme(theme);

        if (!field->isVisible()) {
            continue;
        }

        const float y =
            firstFieldYBase * uiScale +
            static_cast<float>(visibleRow) * theme.lineGap;

        field->setPosition(
            sf::Vector2f(fieldLabelX, y),
            sf::Vector2f(fieldValueX, y)
        );

        ++visibleRow;
    }

    ensureSelectableIndex();
    updateSelectionStates();
}

void SetupScreen::handleKey(sf::Keyboard::Key key) {
    if (fields.empty()) {
        return;
    }

    ensureSelectableIndex();

    if (!hasSelectableField()) {
        return;
    }

    SettingsField& selectedField = *fields[selectedIndex];

    if (selectedField.isEditing()) {
        if (key == sf::Keyboard::Key::Enter) {
            selectedField.onEnter();
            refresh();
        } else if (key == sf::Keyboard::Key::Escape) {
            selectedField.onEscape();
        } else if (key == sf::Keyboard::Key::Backspace) {
            selectedField.onBackspace();
        }

        updateSelectionStates();
        return;
    }

    if (key == sf::Keyboard::Key::Up) {
        moveSelection(-1);
        updateSelectionStates();
        return;
    }

    if (key == sf::Keyboard::Key::Down) {
        moveSelection(1);
        updateSelectionStates();
        return;
    }

    if (key == sf::Keyboard::Key::Left) {
        selectedField.onLeft();
        updateSelectionStates();
        return;
    }

    if (key == sf::Keyboard::Key::Right) {
        selectedField.onRight();
        updateSelectionStates();
        return;
    }

    if (key == sf::Keyboard::Key::Enter) {
        selectedField.onEnter();
        refresh();
        updateSelectionStates();
        return;
    }
}

void SetupScreen::handleTextEntered(char ch) {
    if (fields.empty()) {
        return;
    }

    ensureSelectableIndex();

    if (!hasSelectableField()) {
        return;
    }

    fields[selectedIndex]->onTextEntered(ch);
    updateSelectionStates();
}

bool SetupScreen::isEditing() const {
    if (fields.empty()) {
        return false;
    }

    if (selectedIndex >= fields.size()) {
        return false;
    }

    return fields[selectedIndex]->isEditing();
}

void SetupScreen::refresh() {
    for (const auto& field : fields) {
        field->refresh();
    }

    updateSelectionStates();
}

void SetupScreen::draw(sf::RenderWindow& window) const {
    window.draw(title);

    for (const auto& field : fields) {
        if (field->isVisible()) {
            field->draw(window);
        }
    }
}

float SetupScreen::scale() const {
    return theme.fontSize / 20.f;
}

bool SetupScreen::hasSelectableField() const {
    return std::any_of(fields.begin(), fields.end(), [](const auto& field) {
        return field->isVisible() && field->isSelectable();
    });
}

void SetupScreen::ensureSelectableIndex() {
    if (fields.empty()) {
        selectedIndex = 0;
        return;
    }

    if (selectedIndex >= fields.size()) {
        selectedIndex = 0;
    }

    if (fields[selectedIndex]->isVisible() &&
        fields[selectedIndex]->isSelectable()) {
        return;
    }

    moveSelection(1);
}

void SetupScreen::moveSelection(int direction) {
    if (fields.empty()) {
        return;
    }

    const std::size_t count = fields.size();
    std::size_t index = selectedIndex;

    for (std::size_t step = 0; step < count; ++step) {
        if (direction < 0) {
            index = (index + count - 1) % count;
        } else {
            index = (index + 1) % count;
        }

        if (fields[index]->isVisible() && fields[index]->isSelectable()) {
            selectedIndex = index;
            return;
        }
    }
}

void SetupScreen::updateSelectionStates() {
    for (std::size_t i = 0; i < fields.size(); ++i) {
        if (!fields[i]->isVisible() ||
            !fields[i]->isSelectable() ||
            i != selectedIndex) {
            fields[i]->setState(FieldState::Normal);
        } else if (fields[i]->isEditing()) {
            fields[i]->setState(FieldState::Editing);
        } else {
            fields[i]->setState(FieldState::Selected);
        }
    }
}