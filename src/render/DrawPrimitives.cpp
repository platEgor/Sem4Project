#include "render/DrawPrimitives.hpp"

#include "core/MathUtils.hpp"

#include <algorithm>
#include <cmath>

sf::Vector2f worldToScreen(const Vec2& worldPos, const Camera& camera) {
    return camera.toScreen(worldPos);
}

Vec2 screenToWorld(sf::Vector2f screenPos, const Camera& camera) {
    return camera.toWorld(screenPos);
}

void drawArrow(sf::RenderWindow& window,
               sf::Vector2f from,
               sf::Vector2f to,
               sf::Color color,
               float thickness,
               float headLength,
               float headWidth) {
    const float dx = to.x - from.x;
    const float dy = to.y - from.y;
    const float length = std::sqrt(dx * dx + dy * dy);

    if (length < 1e-3f) {
        return;
    }

    const float angle = std::atan2(dy, dx);
    const float actualHeadLength = std::min(headLength, length * 0.45f);
    const float shaftLength = std::max(0.f, length - actualHeadLength);

    sf::RectangleShape shaft(sf::Vector2f(shaftLength, thickness));
    shaft.setOrigin(sf::Vector2f(0.f, thickness / 2.f));
    shaft.setPosition(from);
    shaft.setRotation(sf::radians(angle));
    shaft.setFillColor(color);

    sf::ConvexShape head(3);
    head.setPoint(0, sf::Vector2f(0.f, 0.f));
    head.setPoint(1, sf::Vector2f(-actualHeadLength, -headWidth / 2.f));
    head.setPoint(2, sf::Vector2f(-actualHeadLength, headWidth / 2.f));
    head.setPosition(to);
    head.setRotation(sf::radians(angle));
    head.setFillColor(color);

    window.draw(shaft);
    window.draw(head);
}

void drawInitialVelocityArrow(sf::RenderWindow& window,
                              const State& initialState,
                              const Camera& camera,
                              float uiScale) {
    constexpr float velocityArrowPreviewSeconds = 300.f;

    const Vec2 arrowStartWorld = initialState.r;
    const Vec2 arrowEndWorld =
        initialState.r + initialState.v * velocityArrowPreviewSeconds;

    const sf::Vector2f arrowStartScreen = camera.toScreen(arrowStartWorld);
    const sf::Vector2f arrowEndScreen = camera.toScreen(arrowEndWorld);

    drawArrow(
        window,
        arrowStartScreen,
        arrowEndScreen,
        sf::Color::Green,
        std::max(1.f, 2.f * uiScale),
        std::max(8.f, 12.f * uiScale),
        std::max(7.f, 10.f * uiScale)
    );
}

void drawThinLine(sf::RenderWindow& window,
                  sf::Vector2f from,
                  sf::Vector2f to,
                  sf::Color color) {
    sf::Vertex line[2];
    line[0].position = from;
    line[0].color = color;
    line[1].position = to;
    line[1].color = color;

    window.draw(line, 2, sf::PrimitiveType::LineStrip);
}

void drawDashedLine(sf::RenderWindow& window,
                    sf::Vector2f from,
                    sf::Vector2f to,
                    sf::Color color,
                    float dashLength,
                    float gapLength) {
    const float dx = to.x - from.x;
    const float dy = to.y - from.y;
    const float length = std::sqrt(dx * dx + dy * dy);

    if (length < 1e-3f) {
        return;
    }

    const sf::Vector2f direction(dx / length, dy / length);
    float current = 0.f;

    while (current < length) {
        const float segmentStart = current;
        const float segmentEnd = std::min(current + dashLength, length);

        const sf::Vector2f dashStart(
            from.x + direction.x * segmentStart,
            from.y + direction.y * segmentStart
        );

        const sf::Vector2f dashEnd(
            from.x + direction.x * segmentEnd,
            from.y + direction.y * segmentEnd
        );

        drawThinLine(window, dashStart, dashEnd, color);

        current += dashLength + gapLength;
    }
}

void drawDashedWorldLine(sf::RenderWindow& window,
                         const Vec2& worldFrom,
                         const Vec2& worldTo,
                         const Camera& camera,
                         sf::Color color,
                         float dashLength,
                         float gapLength) {
    drawDashedLine(
        window,
        worldToScreen(worldFrom, camera),
        worldToScreen(worldTo, camera),
        color,
        dashLength,
        gapLength
    );
}

void drawDashedWorldPolyline(sf::RenderWindow& window,
                             const std::vector<Vec2>& points,
                             const Camera& camera,
                             sf::Color color,
                             float dashLength,
                             float gapLength) {
    if (points.size() < 2) {
        return;
    }

    for (std::size_t i = 1; i < points.size(); ++i) {
        drawDashedWorldLine(
            window,
            points[i - 1],
            points[i],
            camera,
            color,
            dashLength,
            gapLength
        );
    }
}

void drawWorldLabel(sf::RenderWindow& window,
                    const sf::Font& font,
                    const UiTheme& theme,
                    const std::string& label,
                    const Vec2& worldPosition,
                    const Camera& camera,
                    sf::Color color,
                    float offsetX,
                    float offsetY) {
    sf::Text text(font);
    text.setString(label);
    text.setCharacterSize(theme.helpSize);
    text.setFillColor(color);

    const sf::Vector2f screenPosition =
        worldToScreen(worldPosition, camera);

    text.setPosition(sf::Vector2f(
        screenPosition.x + offsetX,
        screenPosition.y + offsetY
    ));

    window.draw(text);
}

void drawWorldArc(sf::RenderWindow& window,
                  const Vec2& center,
                  float radius,
                  float startAngle,
                  float deltaAngle,
                  const Camera& camera,
                  sf::Color color) {
    if (radius <= 0.f || std::abs(deltaAngle) <= 1e-4f) {
        return;
    }

    const int segments = std::clamp(
        static_cast<int>(std::abs(deltaAngle) / twoPiValue * 96.f),
        12,
        128
    );

    std::vector<sf::Vertex> arcVertices;
    arcVertices.reserve(static_cast<std::size_t>(segments + 1));

    for (int i = 0; i <= segments; ++i) {
        const float t = static_cast<float>(i) / static_cast<float>(segments);
        const float angle = startAngle + deltaAngle * t;

        const Vec2 worldPoint{
            center.x + radius * std::cos(angle),
            center.y + radius * std::sin(angle)
        };

        sf::Vertex vertex;
        vertex.position = worldToScreen(worldPoint, camera);
        vertex.color = color;

        arcVertices.push_back(vertex);
    }

    if (arcVertices.size() >= 2) {
        window.draw(
            arcVertices.data(),
            static_cast<int>(arcVertices.size()),
            sf::PrimitiveType::LineStrip
        );
    }
}