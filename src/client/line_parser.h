//
// Created by benoit on 18/10/05.
//

#ifndef QUIZZBOT_LINE_PARSER_H
#define QUIZZBOT_LINE_PARSER_H

#include <string>
#include "common/message.h"

/// Will parse the input string from the GUI and create the corresponding Message
namespace quizzbot{

    constexpr static char CMD_INDICATOR = '/';

    class LineParser {

    public:
        explicit LineParser(std::string name): name_{std::move(name)} {}
        Message parse_line(const std::string& line);

    private:
        std::string name_;
    };
}


#endif //QUIZZBOT_LINE_PARSER_H
