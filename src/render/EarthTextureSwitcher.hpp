#pragma once

#include <SFML/Graphics.hpp>

#include <array>
#include <cstddef>
#include <filesystem>
#include <random>

class EarthTextureSwitcher {
public:
    bool loadFromAssets();

    void applyTo(sf::CircleShape& planet) const;

    void update(sf::CircleShape& planet);

private:
    static std::filesystem::path texturePathForIndex(std::size_t index);

    static int textureWeightForIndex(std::size_t index);

    bool hasAnyLoadedTexture() const;

    void chooseWeightedTexture();

private:
    static constexpr float textureChangeIntervalSeconds = 10.f;

    std::array<sf::Texture, 11> textures;
    std::array<bool, 11> textureLoaded{};

    std::size_t currentTextureIndex = 0;

    std::mt19937 randomEngine{std::random_device{}()};
    sf::Clock textureChangeClock;
};