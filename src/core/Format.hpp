#pragma once

#include <string>

std::string formatFloat(float value, int digits = 1);

std::string trimCopy(const std::string& value);

std::string sanitizeFileStem(const std::string& requestedName);