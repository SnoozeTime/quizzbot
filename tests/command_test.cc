//
// Created by benoit on 18/09/05.
//
#include "gtest/gtest.h"
#include "common/command.h"
using namespace quizzbot;

naive_command_protocol protocol;

TEST(naive_protocol, parse_answer) {
    std::string message = "Athis is my answer";
    Packet packet(message.begin(), message.end());
    std::string expected_content = "this is my answer";
    quizzbot::command cmd;
    protocol.parse(cmd, packet);
    ASSERT_EQ(quizzbot::command::command_type::ANSWER, cmd.type());
    ASSERT_EQ(expected_content, cmd.content());
}

TEST(naive_protocol, parse_message) {
    std::string message = "Mthis is my answer";
    Packet packet(message.begin(), message.end());

    std::string expected_content = "this is my answer";
    quizzbot::command cmd;
    protocol.parse(cmd, packet);
    ASSERT_EQ(quizzbot::command::command_type::MESSAGE, cmd.type());
    ASSERT_EQ(expected_content, cmd.content());
}

TEST(naive_protocol, parse_empty_message) {
    Packet packet = {'M'};
    std::string expected_content;
    quizzbot::command cmd;
    protocol.parse(cmd, packet);
    ASSERT_EQ(quizzbot::command::command_type::MESSAGE, cmd.type());
    ASSERT_EQ(expected_content, cmd.content());
}

TEST(naive_protocol, error_not_enough) {
    Packet packet;
    std::string expected_error = "Message is too short. Requires at least 1 characters";
    quizzbot::command cmd;
    protocol.parse(cmd, packet);
    ASSERT_TRUE(cmd.has_error());
    ASSERT_EQ(expected_error, cmd.content());
}

TEST(naive_protocol, error_unknown_cmd) {
    Packet packet = {'S', 'Z', 'S'};
    std::string expected_error = "Unknown command: S";
    quizzbot::command cmd;
    protocol.parse(cmd, packet);
    ASSERT_TRUE(cmd.has_error());
    ASSERT_EQ(expected_error, cmd.content());
}

TEST(naive_protocol, pack_answer) {
    command cmd(command::command_type::ANSWER, "answer");
    Packet packet = protocol.pack(cmd);
    std::string packet_str(packet.begin(), packet.end());
    ASSERT_STREQ("Aanswer", packet_str.c_str());
}

TEST(naive_protocol, pack_message) {
    command cmd(command::command_type::MESSAGE, "answer");
    Packet packet = protocol.pack(cmd);
    std::string packet_str(packet.begin(), packet.end());
    ASSERT_STREQ("Manswer", packet_str.c_str());
}


TEST(naive_protocol, pack_error) {
    command cmd(command::command_type::ERROR, "answer");
    Packet packet = protocol.pack(cmd);
    std::string packet_str(packet.begin(), packet.end());

    ASSERT_STREQ("Eanswer", packet_str.c_str());
}


TEST(naive_protocol, pack_empty) {
    command cmd(command::command_type::ANSWER, "");
    Packet packet =  protocol.pack(cmd);
    std::string packet_str(packet.begin(), packet.end());
    ASSERT_STREQ("A", packet_str.c_str());
}