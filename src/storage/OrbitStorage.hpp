#pragma once

#include "core/AppTypes.hpp"

#include <filesystem>
#include <optional>
#include <string>
#include <vector>

std::vector<std::filesystem::path> listOrbitFiles();

bool writeKeplerianOrbitTextFile(const std::string& requestedName,
                                 const KeplerianInitialConditions& elements,
                                 std::filesystem::path& savedPath,
                                 std::string& errorMessage);

std::optional<KeplerianInitialConditions> readKeplerianOrbitTextFile(
    const std::filesystem::path& filePath,
    std::string& errorMessage
);