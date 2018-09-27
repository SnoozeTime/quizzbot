//
// Created by benoit on 18/09/28.
//

#ifndef QUIZZBOT_MESSAGE_H
#define QUIZZBOT_MESSAGE_H

//
// Message that can contains different fields of different types.
// first bytes are the message type. Then, each message will use its own specific parsing logic
// based on its fields.
//
#include <stddef.h>
#include "quizzbot_types.h"
#include <sstream>

namespace quizzbot {

    template<int n>
    class message_type_field {

    public:
        parse_code parse(Iter& packet_iterator, size_t len);

    private:
        int type_{0};
        int size_{n};
    };

    class message {

    };

    template<int n>
    parse_code message_type_field<n>::parse(quizzbot::Iter &packet_iterator, size_t len) {

        auto candidate_size = std::string(packet_iterator, packet_iterator+2);
        packet_iterator += 2;
        len -= 2;


        std::istringstream iss(candidate_size);
        size_t size;
        iss >> std::hex >> size;

        if (iss.fail()) {
            return parse_code::BLURP;
        }


        return parse_code::PARSE_OK;
    }

}

#endif //QUIZZBOT_MESSAGE_H
