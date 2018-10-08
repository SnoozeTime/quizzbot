//
// Created by benoit on 18/10/08.
//

#ifndef QUIZZBOT_STRING_UTILS_H
#define QUIZZBOT_STRING_UTILS_H

#include <string>

namespace quizzbot {

    std::string trim_left(const std::string& input) {
        auto idx = input.find_first_not_of(' ');
        if (idx != std::string::npos) {
            return input.substr(idx);
        } else {
            return input;
        }
    }

    std::string trim_right(const std::string& input) {
        auto idx = input.find_last_not_of(' ');
        if (idx != std::string::npos) {
            return input.substr(0, idx+1);
        } else {
            return input;
        }
    }

    std::string trim(const std::string& input) {
        auto trimmed_left = trim_left(input);
        return trim_right(input);
    }

}

#endif //QUIZZBOT_STRING_UTILS_H
