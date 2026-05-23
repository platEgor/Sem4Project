#pragma once

#include "core/AppTypes.hpp"
#include "core/Format.hpp"
#include "ui/LabeledField.hpp"

#include <algorithm>
#include <cmath>
#include <functional>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

template <typename T>
class AdjustableField : public LabeledField {
public:
    using Getter = std::function<T()>;
    using Setter = std::function<void(T)>;
    using Stepper = std::function<T(const T& currentValue, int direction)>;
    using Formatter = std::function<std::string(const T& value)>;
    using ChangeCallback = std::function<void()>;

    AdjustableField(const sf::Font& font,
                    const std::string& label,
                    Getter getter,
                    Setter setter,
                    Stepper stepper,
                    Formatter formatter,
                    ChangeCallback onChanged = {},
                    float textScale = 1.f)
        : LabeledField(font, label, textScale),
          getter(std::move(getter)),
          setter(std::move(setter)),
          stepper(std::move(stepper)),
          formatter(std::move(formatter)),
          onChanged(std::move(onChanged)) {
        refreshValueText();
    }

    void onLeft() override {
        changeByDirection(-1);
    }

    void onRight() override {
        changeByDirection(1);
    }

    void refresh() override {
        refreshValueText();
    }

private:
    void changeByDirection(int direction) {
        if (!getter || !setter || !stepper) {
            return;
        }

        const T newValue = stepper(getter(), direction);

        setter(newValue);
        refreshValueText();

        if (onChanged) {
            onChanged();
        }
    }

    void refreshValueText() {
        if (getter && formatter) {
            valueText.setString(formatter(getter()));
        }
    }

private:
    Getter getter;
    Setter setter;
    Stepper stepper;
    Formatter formatter;
    ChangeCallback onChanged;
};

template <typename T>
typename AdjustableField<T>::Stepper makeOptionsStepper(std::vector<T> options) {
    return [options = std::move(options)](const T& currentValue, int direction) {
        if (options.empty()) {
            return currentValue;
        }

        auto it = std::find(options.begin(), options.end(), currentValue);

        int index = it == options.end()
            ? 0
            : static_cast<int>(std::distance(options.begin(), it));

        index = std::clamp(
            index + direction,
            0,
            static_cast<int>(options.size()) - 1
        );

        return options[static_cast<std::size_t>(index)];
    };
}

template <typename T>
typename AdjustableField<T>::Stepper makeClampedStepStepper(T minValue,
                                                            T maxValue,
                                                            T step) {
    return [minValue, maxValue, step](const T& currentValue, int direction) {
        T value = std::clamp(
            currentValue + static_cast<T>(direction) * step,
            minValue,
            maxValue
        );

        if constexpr (std::is_floating_point_v<T>) {
            if (step > static_cast<T>(0)) {
                const T stepsFromMin =
                    std::round((value - minValue) / step);

                value = std::clamp(
                    minValue + stepsFromMin * step,
                    minValue,
                    maxValue
                );
            }
        }

        return value;
    };
}

inline auto makeFloatFormatter(int digits) {
    return [digits](const float& value) {
        return formatFloat(value, digits);
    };
}

inline auto makeIntFormatter(std::string suffix = {}) {
    return [suffix = std::move(suffix)](const int& value) {
        return std::to_string(value) + suffix;
    };
}

inline auto makeInitialConditionModeFormatter() {
    return [](const InitialConditionMode& mode) {
        return std::string(initialConditionModeToString(mode));
    };
}