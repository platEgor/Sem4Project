#include "render/EarthTextureSwitcher.hpp"

#include <algorithm>
#include <string>
#include <vector>

bool EarthTextureSwitcher::loadFromAssets() {
    bool loadedAnyTexture = false;

    for (std::size_t i = 0; i < textures.size(); ++i) {
        const std::filesystem::path path = texturePathForIndex(i);
        textureLoaded[i] = textures[i].loadFromFile(path.generic_string());

        if (textureLoaded[i]) {
            loadedAnyTexture = true;
        }
    }

    if (textureLoaded[0]) {
        currentTextureIndex = 0;
    } else {
        for (std::size_t i = 0; i < textureLoaded.size(); ++i) {
            if (textureLoaded[i]) {
                currentTextureIndex = i;
                break;
            }
        }
    }

    return loadedAnyTexture;
}

void EarthTextureSwitcher::applyTo(sf::CircleShape& planet) const {
    if (!textureLoaded[currentTextureIndex]) {
        planet.setTexture(nullptr);
        planet.setFillColor(sf::Color::Blue);
        return;
    }

    planet.setFillColor(sf::Color::White);
    planet.setTexture(&textures[currentTextureIndex], true);
}

void EarthTextureSwitcher::update(sf::CircleShape& planet) {
    if (!hasAnyLoadedTexture()) {
        return;
    }

    if (textureChangeClock.getElapsedTime().asSeconds() <
        textureChangeIntervalSeconds) {
        return;
    }

    textureChangeClock.restart();

    chooseWeightedTexture();
    applyTo(planet);
}

std::filesystem::path EarthTextureSwitcher::texturePathForIndex(
    std::size_t index
) {
    if (index == 0) {
        return std::filesystem::path("assets") /
               "textures" /
               "earth" /
               "earth.png";
    }

    return std::filesystem::path("assets") /
           "textures" /
           "earth" /
           ("earth" + std::to_string(index) + ".png");
}

int EarthTextureSwitcher::textureWeightForIndex(std::size_t index) {
    if (index == 0) {
        return 21;
    }

    if (index == 3) {
        return 3;
    }

    if (index == 8) {
        return 5;
    }

    if (index == 10) {
        return 1;
    }

    return 10;
}

bool EarthTextureSwitcher::hasAnyLoadedTexture() const {
    return std::any_of(
        textureLoaded.begin(),
        textureLoaded.end(),
        [](bool loaded) {
            return loaded;
        }
    );
}

void EarthTextureSwitcher::chooseWeightedTexture() {
    std::vector<std::size_t> availableTextureIndices;
    std::vector<int> availableTextureWeights;

    for (std::size_t i = 0; i < textureLoaded.size(); ++i) {
        if (textureLoaded[i]) {
            availableTextureIndices.push_back(i);
            availableTextureWeights.push_back(textureWeightForIndex(i));
        }
    }

    if (availableTextureIndices.empty()) {
        return;
    }

    std::discrete_distribution<std::size_t> textureDistribution(
        availableTextureWeights.begin(),
        availableTextureWeights.end()
    );

    const std::size_t selectedIndexInAvailableList =
        textureDistribution(randomEngine);

    currentTextureIndex =
        availableTextureIndices[selectedIndexInAvailableList];
}