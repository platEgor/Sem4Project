#include "storage/OrbitStorage.hpp"

#include "core/Format.hpp"
#include "core/MathUtils.hpp"

#include <algorithm>
#include <exception>
#include <fstream>
#include <iomanip>

std::vector<std::filesystem::path> listOrbitFiles() {
    std::vector<std::filesystem::path> files;
    const std::filesystem::path directory = "orbits";

    if (!std::filesystem::exists(directory)) {
        return files;
    }

    try {
        for (const auto& entry : std::filesystem::directory_iterator(directory)) {
            if (!entry.is_regular_file()) {
                continue;
            }

            const std::filesystem::path path = entry.path();
            if (path.extension() == ".txt") {
                files.push_back(path);
            }
        }
    } catch (...) {
        return {};
    }

    std::sort(files.begin(), files.end(), [](const auto& lhs, const auto& rhs) {
        return lhs.filename().generic_string() < rhs.filename().generic_string();
    });

    return files;
}

bool writeKeplerianOrbitTextFile(const std::string& requestedName,
                                 const KeplerianInitialConditions& elements,
                                 std::filesystem::path& savedPath,
                                 std::string& errorMessage) {
    const std::string sanitizedName = sanitizeFileStem(requestedName);

    if (sanitizedName.empty()) {
        errorMessage = "empty name";
        return false;
    }

    if (elements.semiMajorAxis <= 0.f ||
        elements.eccentricity < 0.f ||
        elements.eccentricity >= 1.f) {
        errorMessage = "invalid ellipse";
        return false;
    }

    try {
        const std::filesystem::path directory = "orbits";
        std::filesystem::create_directories(directory);

        savedPath = directory / (sanitizedName + ".txt");

        std::ofstream file(savedPath);
        if (!file) {
            errorMessage = "file error";
            return false;
        }

        file << std::setprecision(10);
        file << "OP2D 1\n";
        file << elements.semiMajorAxis << ' '
             << elements.eccentricity << ' '
             << normalizeAnglePositive(elements.argumentOfPeriapsis) << ' '
             << normalizeAnglePositive(elements.trueAnomaly) << ' '
             << (elements.motionDirection >= 0 ? 1 : -1) << '\n';

        return true;
    } catch (const std::exception& error) {
        errorMessage = error.what();
        return false;
    }
}

std::optional<KeplerianInitialConditions> readKeplerianOrbitTextFile(
    const std::filesystem::path& filePath,
    std::string& errorMessage
) {
    std::ifstream file(filePath);

    if (!file) {
        errorMessage = "file error";
        return std::nullopt;
    }

    std::string marker;
    int version = 0;

    if (!(file >> marker >> version) || marker != "OP2D" || version != 1) {
        errorMessage = "invalid format";
        return std::nullopt;
    }

    KeplerianInitialConditions elements;
    int direction = 1;

    if (!(file >> elements.semiMajorAxis
              >> elements.eccentricity
              >> elements.argumentOfPeriapsis
              >> elements.trueAnomaly
              >> direction)) {
        errorMessage = "invalid values";
        return std::nullopt;
    }

    if (elements.semiMajorAxis <= 0.f ||
        elements.eccentricity < 0.f ||
        elements.eccentricity >= 1.f ||
        direction == 0) {
        errorMessage = "invalid orbit";
        return std::nullopt;
    }

    elements.argumentOfPeriapsis =
        normalizeAnglePositive(elements.argumentOfPeriapsis);

    elements.trueAnomaly =
        normalizeAnglePositive(elements.trueAnomaly);

    elements.motionDirection = direction >= 0 ? 1 : -1;

    return elements;
}