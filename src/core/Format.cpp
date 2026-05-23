#include "core/Format.hpp"

#include <cctype>
#include <iomanip>
#include <sstream>

std::string formatFloat(float value, int digits) {
    std::ostringstream out;
    out << std::fixed << std::setprecision(digits) << value;
    return out.str();
}

std::string trimCopy(const std::string& value) {
    std::size_t begin = 0;

    while (begin < value.size() &&
           std::isspace(static_cast<unsigned char>(value[begin]))) {
        ++begin;
    }

    std::size_t end = value.size();

    while (end > begin &&
           std::isspace(static_cast<unsigned char>(value[end - 1]))) {
        --end;
    }

    return value.substr(begin, end - begin);
}

std::string sanitizeFileStem(const std::string& requestedName) {
    std::string stem = trimCopy(requestedName);

    const std::string extension = ".txt";
    if (stem.size() > extension.size() &&
        stem.substr(stem.size() - extension.size()) == extension) {
        stem.resize(stem.size() - extension.size());
        stem = trimCopy(stem);
    }

    std::string sanitized;
    sanitized.reserve(stem.size());

    for (char ch : stem) {
        const unsigned char code = static_cast<unsigned char>(ch);
        const bool isControl = code < 32;
        const bool isForbiddenPathCharacter =
            ch == '/' || ch == '\\' || ch == ':' || ch == '*' ||
            ch == '?' || ch == '"' || ch == '<' || ch == '>' || ch == '|';

        sanitized.push_back(
            (isControl || isForbiddenPathCharacter) ? '_' : ch
        );
    }

    sanitized = trimCopy(sanitized);

    while (sanitized.find("  ") != std::string::npos) {
        sanitized.replace(sanitized.find("  "), 2, " ");
    }

    if (sanitized.size() > 80) {
        sanitized.resize(80);
        sanitized = trimCopy(sanitized);
    }

    return sanitized;
}