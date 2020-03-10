#include <sstream>

#include "util.hpp"

namespace util {
std::vector<std::string> split(const std::string& input, const char delimiter) {
    std::vector<std::string> tokens;
    std::istringstream s(input);

    for (std::string token; std::getline(s, token, delimiter);) {
        tokens.push_back(token);
    }

    return tokens;
}
}
