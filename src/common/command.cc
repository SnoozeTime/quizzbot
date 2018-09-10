//
// Created by benoit on 18/09/05.
//

#include <sstream>
#include "command.h"

namespace quizzbot {

    // If it is a command.
    command::command(command_type type, const std::string& content):
        type_(type),
        content_(content) {

    }

    bool command::has_error() const {
        return type_ == command_type::ERROR;
    }

    command::command_type command::type() const {
        return type_;
    }

    std::string command::content() const {
        return content_;
    }

    void naive_command_protocol::parse(command &cmd, Packet packet) {
        std::string content;
        std::string message(packet.begin(), packet.end());
        auto type = command::command_type::ERROR;
        if (message.empty()) {
            content = "Message is too short. Requires at least 1 characters";
        } else {
            std::string command_str = message.substr(0, 1);
            std::string message_content = message.substr(1);
            if (command_str == "A") {
                type = command::command_type::ANSWER;
                content = message_content;
            } else if (command_str == "M") {
                type = command::command_type::MESSAGE;
                content = message_content;
            } else {
                content = "Unknown command: " + command_str;
            }
        }

        cmd.set_content(std::move(content));
        cmd.set_type(type);
    }

    Packet naive_command_protocol::pack(const quizzbot::command &cmd) {
        std::stringstream cmd_stream;
        switch (cmd.type()) {
            case command::command_type::ANSWER:
                cmd_stream << "A";
                break;
            case command::command_type::MESSAGE:
                cmd_stream << "M";
                break;
            default:
                cmd_stream << "E";
        }

        cmd_stream << cmd.content();
        std::string string_packet = cmd_stream.str();
        return Packet(string_packet.begin(), string_packet.end());
    }
}