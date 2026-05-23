#include <SFML/Graphics.hpp>

#include "core/Vec2.hpp"
#include "core/AppTypes.hpp"
#include "core/MathUtils.hpp"
#include "core/Format.hpp"

#include "physics/Integrator.hpp"
#include "physics/OrbitMath.hpp"
#include "physics/OrbitPrediction.hpp"

#include "ui/UiTheme.hpp"
#include "ui/PopupMessageLayer.hpp"
#include "ui/FloatInputField.hpp"
#include "ui/AdjustableField.hpp"
#include "ui/ReadOnlyTextField.hpp"
#include "ui/TextInputField.hpp"
#include "ui/FileChoiceField.hpp"
#include "ui/ButtonField.hpp"
#include "ui/SetupScreen.hpp"

#include "render/Camera.hpp"
#include "render/DrawPrimitives.hpp"
#include "render/SetupGuides.hpp"
#include "render/OrbitTrail.hpp"
#include "render/ApsisMarkerRenderer.hpp"
#include "render/EarthTextureSwitcher.hpp"

#include "storage/OrbitStorage.hpp"

#include <algorithm>
#include <cmath>
#include <filesystem>
#include <optional>
#include <string>
#include <vector>


int main() {
    float Xres = 1920.f;
    float Yres = 1080.f;
    sf::RenderWindow window(
        sf::VideoMode(sf::Vector2u(Xres, Yres)),
        "OrbitPlotter"
    );
    window.setFramerateLimit(60);

    constexpr float gravitationalConstant = 6.67430e-11f;
    constexpr float earthMass = 5.9722e24f;
    constexpr float earthRadius = 6.371e6f;
    constexpr float initialAltitude = 400000.f;
    constexpr float maxPhysicsStep = 10.f;
    constexpr float gravitationalParameter = gravitationalConstant * earthMass;

    const Vec2 rC{0.f, 0.f};
    const float initialOrbitRadius = earthRadius + initialAltitude;
    const Vec2 r0{7000.f * 1000.f, 0.f};
    const Vec2 v0{0.f, 8.f * 1000.f};
    const float realSecondsPerFrame = 1.f / 60.f;

    State st{rC + r0, v0, {0.f, 0.f}};
    AppMode mode = AppMode::Setup;
    SimulationSettings settings;
    settings.cartesianInitialConditions.x = r0.x;
    settings.cartesianInitialConditions.y = r0.y;
    settings.cartesianInitialConditions.vx = v0.x;
    settings.cartesianInitialConditions.vy = v0.y;

    bool spacecraftCrashed = false;

    constexpr std::size_t trailMaxPoints = 6000;
    constexpr float trailMinPointDistance = 50000.f; // m
    OrbitTrail orbitTrail(trailMaxPoints, trailMinPointDistance, rC);

    constexpr float defaultZoom = 0.00005f;
    Camera camera{{0.f, 0.f}, defaultZoom, Xres, Yres};
    Vec2& cameraCenter = camera.center;
    float& zoom = camera.zoom;
    constexpr float minZoom = 1e-6f;
    constexpr float maxZoom = 1e-3f;

    auto computeSatelliteScreenRadius = [&]() {
        const float zoomFactor = std::sqrt(defaultZoom / zoom);
        return std::clamp(6.f * zoomFactor, 4.f, 30.f);
    };

    const float R_p = earthRadius;
    float R_p_sc = R_p * zoom;

    EarthTextureSwitcher earthTextureSwitcher;
    earthTextureSwitcher.loadFromAssets();

    sf::CircleShape planet(R_p);
    earthTextureSwitcher.applyTo(planet);
    planet.setOrigin(sf::Vector2f(R_p, R_p));
    planet.setPosition(worldToScreen(rC, camera));

    float R_s_sc = computeSatelliteScreenRadius();
    sf::CircleShape satellite(R_s_sc);
    satellite.setFillColor(sf::Color::White);
    satellite.setOrigin(sf::Vector2f(R_s_sc, R_s_sc));
    satellite.setPosition(worldToScreen(st.r, camera));

    sf::Font font;
    if (!font.openFromFile("assets/fonts/Terraria.ttf")) {
        return 1;
    }

    sf::Text modeLabel(font);
    sf::Text modeValue(font);
    sf::Text helpTextS(font);
    sf::Text helpTextR(font);
    sf::Text helpTextP(font);

    UiTheme theme = makeTheme(settings.uiScale);
    SetupScreen setupScreen(font, "Setup Menu");
    SetupScreen runtimeSettingsPanel(font, "Runtime Settings");
    SetupScreen flightInfoPanel(font, "Flight Info");
    ApsisMarkerRenderer apsisMarkerRenderer(font);
    PopupMessageLayer popupMessages(font);

    runtimeSettingsPanel.setBaseLayout(20.f, 70.f, 20.f, 130.f, 190.f);
    flightInfoPanel.setRightAnchoredLayout(Xres, 12.f, 70.f, 270.f, 130.f, 150.f);

    auto applyUiScale = [&]() {
        theme = makeTheme(settings.uiScale);

        modeLabel.setCharacterSize(theme.fontSize);
        modeLabel.setFillColor(sf::Color::White);
        modeLabel.setString("Mode:");
        modeLabel.setPosition(sf::Vector2f(theme.marginX, theme.marginY));

        modeValue.setCharacterSize(theme.fontSize);
        modeValue.setPosition(sf::Vector2f(70.f * settings.uiScale, theme.marginY));

        helpTextS.setCharacterSize(theme.helpSize);
        helpTextS.setFillColor(theme.helpColor);
        helpTextS.setString("Select: Up/Down    Change: Left/Right    Edit/Confirm: Enter    Pan: WASD    Zoom: MouseWheel    Exit: Esc");
        helpTextS.setPosition(sf::Vector2f(theme.marginX, Yres - 2.f * theme.marginY));

        helpTextR.setCharacterSize(theme.helpSize);
        helpTextR.setFillColor(theme.helpColor);
        helpTextR.setString("Settings: Up/Down + Left/Right    Pause: Space    Pan: WASD    Zoom: MouseWheel    Exit: Esc");
        helpTextR.setPosition(sf::Vector2f(theme.marginX, Yres - 2.f * theme.marginY));

        helpTextP.setCharacterSize(theme.helpSize);
        helpTextP.setFillColor(theme.helpColor);
        helpTextP.setString("Settings: Up/Down + Left/Right    Unpause: Space    Reset: R    Pan: WASD    Zoom: MouseWheel    Exit: Esc");
        helpTextP.setPosition(sf::Vector2f(theme.marginX, Yres - 2.f * theme.marginY));

        setupScreen.applyTheme(theme);
        runtimeSettingsPanel.applyTheme(theme);
        flightInfoPanel.applyTheme(theme);
        apsisMarkerRenderer.applyTheme(theme);
        popupMessages.applyTheme(theme);
    };

    auto makeCartesianStateFromSettings = [&]() {
        const Vec2 initialPosition{
            settings.cartesianInitialConditions.x,
            settings.cartesianInitialConditions.y
        };
        const Vec2 initialVelocity{
            settings.cartesianInitialConditions.vx,
            settings.cartesianInitialConditions.vy
        };
        return State{rC + initialPosition, initialVelocity, {0.f, 0.f}};
    };

    auto syncKeplerianFromCartesian = [&]() {
        const State cartesianState = makeCartesianStateFromSettings();
        const std::optional<KeplerianOrbit2D> orbit = cartesianToKeplerianOrbit(
            cartesianState,
            rC,
            gravitationalParameter
        );

        if (!orbit || !orbit->isBoundEllipse) {
            return;
        }

        settings.keplerianInitialConditions.semiMajorAxis = orbit->semiMajorAxis;
        settings.keplerianInitialConditions.eccentricity = orbit->eccentricity;
        settings.keplerianInitialConditions.argumentOfPeriapsis = normalizeAnglePositive(orbit->argumentOfPeriapsis);
        settings.keplerianInitialConditions.trueAnomaly = normalizeAnglePositive(orbit->trueAnomaly);
        settings.keplerianInitialConditions.motionDirection = orbit->specificAngularMomentum >= 0.f ? 1 : -1;
    };

    auto syncCartesianFromKeplerian = [&]() {
        const std::optional<State> keplerianState = keplerianToCartesianState(
            settings.keplerianInitialConditions,
            rC,
            gravitationalParameter
        );

        if (!keplerianState) {
            return;
        }

        settings.cartesianInitialConditions.x = keplerianState->r.x - rC.x;
        settings.cartesianInitialConditions.y = keplerianState->r.y - rC.y;
        settings.cartesianInitialConditions.vx = keplerianState->v.x;
        settings.cartesianInitialConditions.vy = keplerianState->v.y;
    };

    syncKeplerianFromCartesian();

    auto makeInitialState = [&]() {
        if (settings.initialConditionMode == InitialConditionMode::Cartesian) {
            return makeCartesianStateFromSettings();
        }

        const std::optional<State> keplerianState = keplerianToCartesianState(
            settings.keplerianInitialConditions,
            rC,
            gravitationalParameter
        );

        if (keplerianState) {
            return *keplerianState;
        }

        return State{rC + r0, v0, {0.f, 0.f}};
    };

    auto updateSpacecraftCrashState = [&]() {
        spacecraftCrashed = (st.r - rC).length() <= earthRadius;
        satellite.setFillColor(spacecraftCrashed ? sf::Color(255, 0, 0) : sf::Color::White);
    };

    auto resetSimulation = [&]() {
        st = makeInitialState();
        orbitTrail.reset(st.r);
        updateSpacecraftCrashState();
        satellite.setPosition(worldToScreen(st.r, camera));
    };

    auto refreshSettingsPanels = [&]() {
        setupScreen.refresh();
        runtimeSettingsPanel.refresh();
        flightInfoPanel.refresh();
    };

    auto updateSetupPreview = [&]() {
        if (mode == AppMode::Setup) {
            st = makeInitialState();
            orbitTrail.clear();
            updateSpacecraftCrashState();
        }

        refreshSettingsPanels();
    };

    auto showCartesianInitialConditionFields = [&]() {
        return settings.initialConditionMode == InitialConditionMode::Cartesian;
    };

    auto showKeplerianInitialConditionFields = [&]() {
        return settings.initialConditionMode == InitialConditionMode::Keplerian;
    };

    std::vector<std::filesystem::path> savedOrbitFiles;
    auto reloadSavedOrbitFiles = [&]() {
        savedOrbitFiles = listOrbitFiles();
    };
    reloadSavedOrbitFiles();

    auto currentKeplerianElementsForSaving = [&]() -> std::optional<KeplerianInitialConditions> {
        if (settings.initialConditionMode == InitialConditionMode::Keplerian) {
            KeplerianInitialConditions elements = settings.keplerianInitialConditions;
            elements.argumentOfPeriapsis = normalizeAnglePositive(elements.argumentOfPeriapsis);
            elements.trueAnomaly = normalizeAnglePositive(elements.trueAnomaly);
            elements.motionDirection = elements.motionDirection >= 0 ? 1 : -1;
            return elements;
        }

        const State cartesianState = makeCartesianStateFromSettings();
        const std::optional<KeplerianOrbit2D> orbit = cartesianToKeplerianOrbit(
            cartesianState,
            rC,
            gravitationalParameter
        );

        if (!orbit || !orbit->isBoundEllipse) {
            return std::nullopt;
        }

        KeplerianInitialConditions elements;
        elements.semiMajorAxis = orbit->semiMajorAxis;
        elements.eccentricity = orbit->eccentricity;
        elements.argumentOfPeriapsis = normalizeAnglePositive(orbit->argumentOfPeriapsis);
        elements.trueAnomaly = normalizeAnglePositive(orbit->trueAnomaly);
        elements.motionDirection = orbit->specificAngularMomentum >= 0.f ? 1 : -1;
        return elements;
    };

    auto saveCurrentOrbit = [&](const std::string& requestedName) {
        const std::optional<KeplerianInitialConditions> elements = currentKeplerianElementsForSaving();

        if (!elements) {
            popupMessages.show("Save failed: not an ellipse", sf::Color(255, 120, 120));
            refreshSettingsPanels();
            return;
        }

        std::filesystem::path savedPath;
        std::string errorMessage;
        if (writeKeplerianOrbitTextFile(requestedName, *elements, savedPath, errorMessage)) {
            reloadSavedOrbitFiles();
            popupMessages.show("Saved: " + savedPath.filename().generic_string(), sf::Color::Green);
        } else {
            popupMessages.show("Save failed: " + errorMessage, sf::Color(255, 120, 120));
        }

        refreshSettingsPanels();
    };

    auto loadOrbitFromFile = [&](const std::filesystem::path& filePath) {
        std::string errorMessage;
        const std::optional<KeplerianInitialConditions> loadedElements = readKeplerianOrbitTextFile(filePath, errorMessage);

        if (!loadedElements) {
            reloadSavedOrbitFiles();
            popupMessages.show("Load failed: " + errorMessage, sf::Color(255, 120, 120));
            refreshSettingsPanels();
            return;
        }

        settings.keplerianInitialConditions = *loadedElements;
        settings.initialConditionMode = InitialConditionMode::Keplerian;
        syncCartesianFromKeplerian();
        setupScreen.applyTheme(theme);
        updateSetupPreview();
        popupMessages.show("Loaded: " + filePath.filename().generic_string(), sf::Color::Green);
    };

    addField<AdjustableField<float>>(
        setupScreen,
        font,
        "UI Scale",
        [&]() { return settings.uiScale; },
        [&](float value) { settings.uiScale = value; },
        makeClampedStepStepper<float>(0.0f, 5.0f, 0.1f),
        makeFloatFormatter(1),
        [&]() { applyUiScale(); }
    );

    addField<AdjustableField<int>>(
        setupScreen,
        font,
        "Time Warp",
        [&]() { return settings.timeWarp; },
        [&](int value) { settings.timeWarp = value; },
        makeOptionsStepper<int>(
            std::vector<int>{1, 5, 10, 100, 1000, 10000, 100000}
        ),
        makeIntFormatter("x"),
        [&]() { refreshSettingsPanels(); }
    );

    addField<AdjustableField<InitialConditionMode>>(
        setupScreen,
        font,
        "Initial Cond.",
        [&]() { return settings.initialConditionMode; },
        [&](InitialConditionMode value) {
            if (value == settings.initialConditionMode) {
                return;
            }

            if (value == InitialConditionMode::Keplerian) {
                syncKeplerianFromCartesian();
            } else {
                syncCartesianFromKeplerian();
            }

            settings.initialConditionMode = value;
        },
        makeOptionsStepper<InitialConditionMode>(
            std::vector<InitialConditionMode>{
                InitialConditionMode::Cartesian,
                InitialConditionMode::Keplerian
            }
        ),
        [](const InitialConditionMode& value) {
            return std::string(initialConditionModeToString(value));
        },
        [&]() {
            setupScreen.applyTheme(theme);
            updateSetupPreview();
        },
        1.25f
    );

    addField<FloatInputField>(setupScreen, 
        font,
        "x, km",
        [&]() { return settings.cartesianInitialConditions.x / 1000.f; },
        [&](float value) { settings.cartesianInitialConditions.x = value * 1000.f; },
        -100000.f,
        100000.f,
        1,
        [&]() { updateSetupPreview(); },
        showCartesianInitialConditionFields
    );

    addField<FloatInputField>(setupScreen, 
        font,
        "y, km",
        [&]() { return settings.cartesianInitialConditions.y / 1000.f; },
        [&](float value) { settings.cartesianInitialConditions.y = value * 1000.f; },
        -100000.f,
        100000.f,
        1,
        [&]() { updateSetupPreview(); },
        showCartesianInitialConditionFields
    );

    addField<FloatInputField>(setupScreen, 
        font,
        "Vx, km/s",
        [&]() { return settings.cartesianInitialConditions.vx / 1000.f; },
        [&](float value) { settings.cartesianInitialConditions.vx = value * 1000.f; },
        -100.f,
        100.f,
        3,
        [&]() { updateSetupPreview(); },
        showCartesianInitialConditionFields
    );

    addField<FloatInputField>(setupScreen, 
        font,
        "Vy, km/s",
        [&]() { return settings.cartesianInitialConditions.vy / 1000.f; },
        [&](float value) { settings.cartesianInitialConditions.vy = value * 1000.f; },
        -100.f,
        100.f,
        3,
        [&]() { updateSetupPreview(); },
        showCartesianInitialConditionFields
    );

    addField<FloatInputField>(setupScreen, 
        font,
        "a, km",
        [&]() { return settings.keplerianInitialConditions.semiMajorAxis / 1000.f; },
        [&](float value) { settings.keplerianInitialConditions.semiMajorAxis = value * 1000.f; },
        1.f,
        1000000.f,
        1,
        [&]() { updateSetupPreview(); },
        showKeplerianInitialConditionFields
    );

    addField<FloatInputField>(setupScreen, 
        font,
        "e",
        [&]() { return settings.keplerianInitialConditions.eccentricity; },
        [&](float value) { settings.keplerianInitialConditions.eccentricity = std::clamp(value, 0.f, 0.9999f); },
        0.f,
        0.9999f,
        4,
        [&]() { updateSetupPreview(); },
        showKeplerianInitialConditionFields
    );

    addField<FloatInputField>(setupScreen, 
        font,
        "Arg peri., deg",
        [&]() { return radiansToDegrees(normalizeAnglePositive(settings.keplerianInitialConditions.argumentOfPeriapsis)); },
        [&](float value) { settings.keplerianInitialConditions.argumentOfPeriapsis = normalizeAnglePositive(degreesToRadians(value)); },
        -360.f,
        360.f,
        1,
        [&]() { updateSetupPreview(); },
        showKeplerianInitialConditionFields
    );

    addField<FloatInputField>(setupScreen, 
        font,
        "True anom., deg",
        [&]() { return radiansToDegrees(normalizeAnglePositive(settings.keplerianInitialConditions.trueAnomaly)); },
        [&](float value) { settings.keplerianInitialConditions.trueAnomaly = normalizeAnglePositive(degreesToRadians(value)); },
        -360.f,
        360.f,
        1,
        [&]() { updateSetupPreview(); },
        showKeplerianInitialConditionFields
    );

    addField<AdjustableField<float>>(
        runtimeSettingsPanel,
        font,
        "UI Scale",
        [&]() { return settings.uiScale; },
        [&](float value) { settings.uiScale = value; },
        makeClampedStepStepper<float>(0.0f, 5.0f, 0.1f),
        makeFloatFormatter(1),
        [&]() { applyUiScale(); }
    );

    addField<AdjustableField<int>>(
        runtimeSettingsPanel,
        font,
        "Time Warp",
        [&]() { return settings.timeWarp; },
        [&](int value) { settings.timeWarp = value; },
        makeOptionsStepper<int>(
            std::vector<int>{1, 5, 10, 100, 1000, 10000, 100000}
        ),
        makeIntFormatter("x"),
        [&]() { refreshSettingsPanels(); }
    );

    addField<ReadOnlyTextField>(flightInfoPanel, 
        font,
        "Orbit Radius",
        [&]() {
            const float radiusKm = (st.r - rC).length() / 1000.f;
            return formatFloat(radiusKm, 1) + " km";
        }
    );

    addField<ReadOnlyTextField>(flightInfoPanel, 
        font,
        "Speed",
        [&]() {
            const float speedKmPerSec = st.v.length() / 1000.f;
            return formatFloat(speedKmPerSec, 3) + " km/s";
        }
    );

    addField<ButtonField>(setupScreen, 
        font,
        "Start",
        [&]() {
            resetSimulation();
            mode = AppMode::Running;
        }
    );

    addField<TextInputField>(setupScreen, 
        font,
        "Save Orbit",
        "<name.txt>",
        [&](const std::string& requestedName) { saveCurrentOrbit(requestedName); }
    );

    addField<FileChoiceField>(setupScreen, 
        font,
        "Load Orbit",
        [&]() { return savedOrbitFiles; },
        [&](const std::filesystem::path& filePath) { loadOrbitFromFile(filePath); }
    );

    applyUiScale();

    while (window.isOpen()) {
        while (auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }

            if (const auto* scroll = event->getIf<sf::Event::MouseWheelScrolled>()) {
                if (scroll->delta > 0) {
                    zoom *= 1.1f;
                } else if (scroll->delta < 0) {
                    zoom /= 1.1f;
                }

                if (zoom < minZoom) {
                    zoom = minZoom;
                }
                if (zoom > maxZoom) {
                    zoom = maxZoom;
                }
            }

            if (const auto* key = event->getIf<sf::Event::KeyPressed>()) {
                if (key->code == sf::Keyboard::Key::Escape &&
                    !(mode == AppMode::Setup && setupScreen.isEditing())) {
                    window.close();
                }

                float panSpeed = 10.f / zoom;
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A)) {
                    cameraCenter.x -= panSpeed;
                }
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D)) {
                    cameraCenter.x += panSpeed;
                }
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W)) {
                    cameraCenter.y -= panSpeed;
                }
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S)) {
                    cameraCenter.y += panSpeed;
                }

                switch (mode) {
                    case AppMode::Setup:
                        setupScreen.handleKey(key->code);
                        break;

                    case AppMode::Running:
                        runtimeSettingsPanel.handleKey(key->code);
                        if (key->code == sf::Keyboard::Key::Space) {
                            mode = AppMode::Paused;
                        }
                        break;

                    case AppMode::Paused:
                        runtimeSettingsPanel.handleKey(key->code);
                        if (key->code == sf::Keyboard::Key::Space) {
                            mode = AppMode::Running;
                        }
                        if (key->code == sf::Keyboard::Key::R) {
                            resetSimulation();
                            mode = AppMode::Setup;
                        }
                        break;
                }
            }

            if (const auto* text = event->getIf<sf::Event::TextEntered>()) {
                if (mode == AppMode::Setup && setupScreen.isEditing()) {
                    char ch = static_cast<char>(text->unicode);
                    setupScreen.handleTextEntered(ch);
                }
            }
        }

        popupMessages.update(realSecondsPerFrame);

        if (mode == AppMode::Running && !spacecraftCrashed) {
            float simulatedSecondsThisFrame = realSecondsPerFrame * static_cast<float>(settings.timeWarp);

            while (simulatedSecondsThisFrame > 0.f && !spacecraftCrashed) {
                if ((st.r - rC).length() <= earthRadius) {
                    updateSpacecraftCrashState();
                    break;
                }

                const float physicsStep = std::min(maxPhysicsStep, simulatedSecondsThisFrame);
                stepSemiImplicitEuler(st, physicsStep, rC, gravitationalConstant, earthMass);

                if ((st.r - rC).length() <= earthRadius) {
                    updateSpacecraftCrashState();
                    break;
                }

                orbitTrail.addIfFarEnough(st.r);
                simulatedSecondsThisFrame -= physicsStep;
            }
        }

        earthTextureSwitcher.update(planet);

        R_p_sc = R_p * zoom;
        planet.setRadius(R_p_sc);
        planet.setOrigin(sf::Vector2f(R_p_sc, R_p_sc));

        R_s_sc = computeSatelliteScreenRadius();
        satellite.setRadius(R_s_sc);
        satellite.setOrigin(sf::Vector2f(R_s_sc, R_s_sc));

        planet.setPosition(worldToScreen(rC, camera));
        satellite.setPosition(worldToScreen(st.r, camera));

        modeValue.setString(modeToString(mode));
        modeValue.setFillColor(modeToColor(mode));

        if (mode != AppMode::Setup) {
            runtimeSettingsPanel.refresh();
            flightInfoPanel.refresh();
        }

        window.clear(sf::Color(20, 30, 50));

        const bool isSetupMode = mode == AppMode::Setup;
        const State setupInitialState = isSetupMode ? makeInitialState() : st;

        if (isSetupMode) {
            const std::vector<Vec2> predictedTrajectory = predictSetupTrajectory(
                setupInitialState,
                rC,
                gravitationalConstant,
                earthMass,
                earthRadius
            );

            drawDashedWorldPolyline(
                window,
                predictedTrajectory,
                camera,
                sf::Color(0, 255, 0, 210),
                std::max(5.f, 8.f * settings.uiScale),
                std::max(4.f, 6.f * settings.uiScale)
            );
        }

        orbitTrail.draw(window, camera);

        if (const auto apsisPoints = orbitTrail.findApsisPoints(rC)) {
            apsisMarkerRenderer.draw(
                window,
                *apsisPoints,
                camera,
                settings.uiScale
            );
        }

        window.draw(planet);

        if (isSetupMode && settings.initialConditionMode == InitialConditionMode::Cartesian) {
            drawCartesianSetupGuides(
                window,
                font,
                theme,
                setupInitialState,
                rC,
                camera,
                settings.uiScale
            );
        } else if (isSetupMode && settings.initialConditionMode == InitialConditionMode::Keplerian) {
            drawKeplerianSetupGuides(
                window,
                font,
                theme,
                setupInitialState,
                rC,
                gravitationalParameter,
                earthRadius,
                camera,
                settings.uiScale
            );
        }

        if (isSetupMode) {
            drawInitialVelocityArrow(
                window,
                setupInitialState,
                camera,
                settings.uiScale
            );
        }

        window.draw(satellite);

        window.draw(modeLabel);
        window.draw(modeValue);

        switch (mode) {
            case AppMode::Setup:
                setupScreen.draw(window);
                window.draw(helpTextS);
                break;

            case AppMode::Running:
                flightInfoPanel.draw(window);
                runtimeSettingsPanel.draw(window);
                window.draw(helpTextR);
                break;

            case AppMode::Paused:
                flightInfoPanel.draw(window);
                runtimeSettingsPanel.draw(window);
                window.draw(helpTextP);
                break;
        }

        popupMessages.draw(window, Xres, settings.uiScale);

        window.display();
    }

    return 0;
}
