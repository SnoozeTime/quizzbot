//
// Created by benoit on 18/10/05.
//

#include "line_parser.h"

#include <sstream>

namespace quizzbot {
    Message LineParser::parse_line(const std::string &line) {
        // If empty, error. Should not pass the UI class anyway...
        if (line.empty()) {
            return Message{std::make_unique<ErrorMessage>("Line shouldn't be empty.")};
        }

        // If first character is not slash, just send as a message.
        if (line.at(0) != CMD_INDICATOR) {
            return Message{std::make_unique<MessageMessage>(name_, line)};
        }

        // Get first word.
        std::stringstream ss;
        ss << line;

        std::string cmd;
        ss >> cmd;
        
        // Check it is a valid command.

        return Message{std::make_unique<ErrorMessage>("Couldn't find command.")};
    }
}