//
// Created by benoit on 18/09/12.
//

#include "protocol.h"

namespace quizzbot {

    int compute_cc(const Packet& content) {
        int sum = 0;
        for (const auto& b: content) {
            sum += b;
        }
        return sum % 255;
    }
    parse_code checksum_parser::read(const Packet& msg_bytes_, Iter& input_iter, size_t len) {
        if (len < 2) {
            return parse_code::NOT_ENOUGH;
        }

        std::string checksum(input_iter, input_iter+2);
        input_iter += 2;


        int expected_checksum = compute_cc(msg_bytes_);
        std::stringstream ss;
        ss << std::setfill('0') << std::setw(2) << std::hex << expected_checksum;

        if (checksum != ss.str()) {
            return parse_code::BAD_CHECKSUM;
        }

        return parse_code::PARSE_OK;
    }
}