//
// Created by benoit on 18/10/05.
//

#include <gtest/gtest.h>
#include "client/line_parser.h"
#include <string>
#include "common/messages/ErrorMessage.h"
#include "common/messages/MessageMessage.h"

using namespace quizzbot;

TEST(line_parser, empty_line_error) {
    std::string line = "";
    LineParser parser{"MyName"};
    auto  msg = parser.parse_line(line);

    ASSERT_EQ(MessageType::ERROR, msg.message_type());
    auto content = dynamic_cast<const ErrorMessage*>(msg.data());
    ASSERT_STREQ("Line shouldn't be empty.", content->error().c_str());
}

// When putting a slash with nothing or unknown command.
TEST(line_parser, malformed_cmd_error) {

}

TEST(line_parser, parse_normal_message) {
    std::string line = "Bonjour c'est une ligne.";
    LineParser parser{"MyName"};
    auto  msg = parser.parse_line(line);

    ASSERT_EQ(MessageType::MESSAGE, msg.message_type());
    auto content = dynamic_cast<const MessageMessage*>(msg.data());
    ASSERT_STREQ(line.c_str(), content->msg().c_str());
    ASSERT_STREQ("MyName", content->from().c_str());
}

TEST(line_parser, parse_answer_short_cmd) {
    std::string line = "/a Bonjour c'est une ligne.";
    LineParser parser{"MyName"};
    auto  msg = parser.parse_line(line);

    ASSERT_EQ(MessageType::ANSWER, msg.message_type());
}

TEST(line_parser, parse_answer_long_cmd) {
    std::string line = "/answer Bonjour c'est une ligne.";
    LineParser parser{"MyName"};
    auto  msg = parser.parse_line(line);

    ASSERT_EQ(MessageType::ANSWER, msg.message_type());
}

