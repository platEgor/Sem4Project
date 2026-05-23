#include "render/SetupGuides.hpp"

#include "core/MathUtils.hpp"
#include "physics/OrbitMath.hpp"
#include "render/DrawPrimitives.hpp"

#include <algorithm>
#include <cmath>
#include <optional>

void drawCartesianSetupGuides(sf::RenderWindow& window,
                              const sf::Font& font,
                              const UiTheme& theme,
                              const State& initialState,
                              const Vec2& origin,
                              const Camera& camera,
                              float uiScale) {
    const Vec2 worldTopLeft = screenToWorld(
        sf::Vector2f(0.f, 0.f),
        camera
    );

    const Vec2 worldBottomRight = screenToWorld(
        sf::Vector2f(camera.screenWidth, camera.screenHeight),
        camera
    );

    const float minX = std::min(worldTopLeft.x, worldBottomRight.x);
    const float maxX = std::max(worldTopLeft.x, worldBottomRight.x);
    const float minY = std::min(worldTopLeft.y, worldBottomRight.y);
    const float maxY = std::max(worldTopLeft.y, worldBottomRight.y);

    const sf::Color axisColor = sf::Color::White;
    const sf::Color guideColor = sf::Color(255, 255, 255, 180);

    const Vec2 xAxisStart{minX, origin.y};
    const Vec2 xAxisEnd{maxX, origin.y};
    const Vec2 yAxisStart{origin.x, minY};
    const Vec2 yAxisEnd{origin.x, maxY};

    drawThinLine(
        window,
        worldToScreen(xAxisStart, camera),
        worldToScreen(xAxisEnd, camera),
        axisColor
    );

    drawThinLine(
        window,
        worldToScreen(yAxisStart, camera),
        worldToScreen(yAxisEnd, camera),
        axisColor
    );

    const float arrowLengthScreen = std::max(18.f, 26.f * uiScale);
    const float arrowThickness = 1.f;
    const float arrowHeadLength = std::max(6.f, 8.f * uiScale);
    const float arrowHeadWidth = std::max(5.f, 7.f * uiScale);

    const sf::Vector2f xArrowEnd = worldToScreen(xAxisEnd, camera);
    const sf::Vector2f xArrowStart(
        xArrowEnd.x - arrowLengthScreen,
        xArrowEnd.y
    );

    drawArrow(
        window,
        xArrowStart,
        xArrowEnd,
        axisColor,
        arrowThickness,
        arrowHeadLength,
        arrowHeadWidth
    );

    const sf::Vector2f yArrowEnd = worldToScreen(yAxisEnd, camera);
    const sf::Vector2f yArrowStart(
        yArrowEnd.x,
        yArrowEnd.y - arrowLengthScreen
    );

    drawArrow(
        window,
        yArrowStart,
        yArrowEnd,
        axisColor,
        arrowThickness,
        arrowHeadLength,
        arrowHeadWidth
    );

    sf::Text xLabel(font);
    xLabel.setString("x");
    xLabel.setCharacterSize(theme.helpSize);
    xLabel.setFillColor(axisColor);
    xLabel.setPosition(sf::Vector2f(
        xArrowEnd.x - 16.f * uiScale,
        xArrowEnd.y + 4.f * uiScale
    ));
    window.draw(xLabel);

    sf::Text yLabel(font);
    yLabel.setString("y");
    yLabel.setCharacterSize(theme.helpSize);
    yLabel.setFillColor(axisColor);
    yLabel.setPosition(sf::Vector2f(
        yArrowEnd.x + 6.f * uiScale,
        yArrowEnd.y - 22.f * uiScale
    ));
    window.draw(yLabel);

    const Vec2 satellitePosition = initialState.r;
    const Vec2 projectionOnXAxis{satellitePosition.x, origin.y};
    const Vec2 projectionOnYAxis{origin.x, satellitePosition.y};

    const float dashLength = std::max(5.f, 8.f * uiScale);
    const float gapLength = std::max(4.f, 6.f * uiScale);

    drawDashedWorldLine(
        window,
        satellitePosition,
        projectionOnXAxis,
        camera,
        guideColor,
        dashLength,
        gapLength
    );

    drawDashedWorldLine(
        window,
        satellitePosition,
        projectionOnYAxis,
        camera,
        guideColor,
        dashLength,
        gapLength
    );
}

void drawKeplerianSetupGuides(sf::RenderWindow& window,
                              const sf::Font& font,
                              const UiTheme& theme,
                              const State& initialState,
                              const Vec2& focus,
                              float gravitationalParameter,
                              float centralBodyRadius,
                              const Camera& camera,
                              float uiScale) {
    const std::optional<KeplerianOrbit2D> orbit = cartesianToKeplerianOrbit(
        initialState,
        focus,
        gravitationalParameter
    );

    if (!orbit || !orbit->isBoundEllipse) {
        return;
    }

    const Vec2 relativePosition = initialState.r - focus;
    const float currentRadius =
        std::max(relativePosition.length(), centralBodyRadius * 1.2f);

    const float apoapsisRadius =
        orbit->semiMajorAxis * (1.f + orbit->eccentricity);

    const float periapsisRadius =
        orbit->semiMajorAxis * (1.f - orbit->eccentricity);

    const float lineLength = std::max({
        currentRadius,
        apoapsisRadius,
        centralBodyRadius * 1.6f
    });

    const float referenceLineLength = lineLength * 1.45f;

    const float arcRadius = std::clamp(
        std::min(currentRadius, apoapsisRadius) * 0.25f,
        centralBodyRadius * 0.25f,
        std::max(centralBodyRadius * 0.35f, apoapsisRadius * 0.35f)
    );

    const float trueAnomalyArcRadius = arcRadius * 1.35f;

    const sf::Color guideColor = sf::Color(255, 255, 255, 190);
    const sf::Color arcColor = sf::Color(255, 255, 255, 220);
    const sf::Color referenceColor = sf::Color(255, 255, 255, 210);

    const Vec2 referenceDirection{1.f, 0.f};
    const Vec2 referenceEnd =
        focus + referenceDirection * referenceLineLength;

    const Vec2 periapsisPoint =
        focus + orbit->periapsisDirection * periapsisRadius;

    drawArrow(
        window,
        worldToScreen(focus, camera),
        worldToScreen(referenceEnd, camera),
        referenceColor,
        1.f,
        std::max(6.f, 8.f * uiScale),
        std::max(5.f, 7.f * uiScale)
    );

    drawThinLine(
        window,
        worldToScreen(focus, camera),
        worldToScreen(periapsisPoint, camera),
        guideColor
    );

    drawDashedWorldLine(
        window,
        focus,
        initialState.r,
        camera,
        guideColor,
        std::max(5.f, 8.f * uiScale),
        std::max(4.f, 6.f * uiScale)
    );

    const float argumentOfPeriapsis =
        normalizeAnglePositive(orbit->argumentOfPeriapsis);

    const int directionSign =
        orbit->specificAngularMomentum >= 0.f ? 1 : -1;

    const float trueAnomaly =
        normalizeAnglePositive(orbit->trueAnomaly);

    drawWorldArc(
        window,
        focus,
        arcRadius,
        0.f,
        argumentOfPeriapsis,
        camera,
        arcColor
    );

    drawWorldArc(
        window,
        focus,
        trueAnomalyArcRadius,
        argumentOfPeriapsis,
        static_cast<float>(directionSign) * trueAnomaly,
        camera,
        arcColor
    );

    const Vec2 referenceLabelPosition =
        focus + referenceDirection * (referenceLineLength * 0.9f);

    const Vec2 periapsisLabelPosition =
        focus + orbit->periapsisDirection * (periapsisRadius * 1.05f);

    drawWorldLabel(
        window,
        font,
        theme,
        "ref",
        referenceLabelPosition,
        camera,
        referenceColor,
        4.f * uiScale,
        4.f * uiScale
    );

    drawWorldLabel(
        window,
        font,
        theme,
        "peri",
        periapsisLabelPosition,
        camera,
        guideColor,
        4.f * uiScale,
        4.f * uiScale
    );
}