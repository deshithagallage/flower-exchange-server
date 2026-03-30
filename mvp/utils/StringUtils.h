#pragma once

#include <string>
#include <vector>
#include <sstream>
#include <algorithm>

namespace flower_exchange {

/** String manipulation helpers */

// Split string by delimiter, e.g., "a,b,c" -> {"a","b","c"}
inline std::vector<std::string> split(const std::string& str, const std::string& delimiter = ",") {
    std::vector<std::string> parts;
    if (str.empty()) return parts;
    size_t start = 0;
    size_t end = str.find(delimiter);
    while (end != std::string::npos) {
        parts.push_back(str.substr(start, end - start));
        start = end + delimiter.length();
        end = str.find(delimiter, start);
    }
    parts.push_back(str.substr(start));
    return parts;
}

// Trim leading whitespace, e.g., "  hello" -> "hello"
inline std::string trimLeft(const std::string& str) {
    size_t start = 0;
    while (start < str.length() && std::isspace(str[start])) start++;
    return str.substr(start);
}

// Trim trailing whitespace, e.g., "hello  " -> "hello"
inline std::string trimRight(const std::string& str) {
    size_t end = str.length();
    while (end > 0 && std::isspace(str[end - 1])) end--;
    return str.substr(0, end);
}

// Trim leading and trailing whitespace, e.g., "  hi  " -> "hi"
inline std::string trim(const std::string& str) {
    return trimRight(trimLeft(str));
}

// Convert string to uppercase, e.g., "hi" -> "HI"
inline std::string toUpper(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::toupper);
    return result;
}

// Convert string to lowercase, e.g., "HI" -> "hi"
inline std::string toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

// Convert string to int, e.g., "42" -> 42
inline int stringToInt(const std::string& str) {
    try {
        return std::stoi(str);
    } catch (...) {
        throw std::invalid_argument("Cannot convert '" + str + "' to int");
    }
}

// Convert string to double, e.g., "3.14" -> 3.14
inline double stringToDouble(const std::string& str) {
    try {
        return std::stod(str);
    } catch (...) {
        throw std::invalid_argument("Cannot convert '" + str + "' to double");
    }
}

// Join vector of strings with delimiter, e.g., {"a","b"} -> "a,b"
inline std::string join(const std::vector<std::string>& parts, const std::string& delimiter = ",") {
    if (parts.empty()) return "";
    std::ostringstream oss;
    for (size_t i = 0; i < parts.size(); ++i) {
        oss << parts[i];
        if (i < parts.size() - 1) oss << delimiter;
    }
    return oss.str();
}

// Check if string contains only digits, e.g., "123" -> true
inline bool isNumeric(const std::string& str) {
    if (str.empty()) return false;
    return std::all_of(str.begin(), str.end(), ::isdigit);
}

} 