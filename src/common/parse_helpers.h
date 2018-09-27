//
// Created by benoit on 18/09/28.
//

#ifndef QUIZZBOT_PARSE_HELPERS_H
#define QUIZZBOT_PARSE_HELPERS_H

#include <sstream>
#include <string>

#include "quizzbot_types.h"

namespace quizzbot {

    // will parse the length for the given size but will not modify the input iterator and size.
    parse_code parse_hex_size(const Iter& input_iter, size_t len, size_t &size, const size_t& field_length) {
        if (len < field_length) {
            return parse_code::NOT_ENOUGH;
        }

        auto candidate_size = std::string(input_iter, input_iter+field_length);

        std::istringstream iss(candidate_size);
        iss >> std::hex >> size;

        if (iss.fail()) {
            return parse_code::BLURP;
        }

        return parse_code::PARSE_OK;
    }

}
#endif //QUIZZBOT_PARSE_HELPERS_H
