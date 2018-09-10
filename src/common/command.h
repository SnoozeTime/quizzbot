//
// Command will
//

#pragma once
#include <string>
#include <memory>
#include "quizzbot_types.h"

/*
    Parse command coming from the main.

    Message coming from a main has the format:
    2 char for the command, content, \r\n
 */
namespace quizzbot {

    enum class parse_code {
        OK,
        NOT_ENOUGH,
        BAD_CHECKSUM,
        BLURP,
    };

    class tcp_connection;
    class command {

    public:

        enum class command_type {
            ERROR,
            ANSWER, // either /a or /answer
            MESSAGE,

            EMPTY,
        };

        command(): type_(command_type::EMPTY), content_("") {}
        // If it is a command.
        command(command_type type, const std::string& content);

        bool has_error() const;
        command_type type() const;
        std::string content() const;
        void set_type(command_type type){ type_ = type;}
        void set_content(std::string &&content) { content_ = std::move(content); }
    private:

        // ------------------------------------
        command_type type_;
        std::string content_;
    };

    /**
     * Naive protocol. The first char will correspond to a command, Rest of the string is the content.
     * A-> answer
     * B -> chat message
     *
     * @param message
     * @return
     */
     class naive_command_protocol {
     public:
         void parse(command & cmd, Packet packet);
         Packet pack(const command& cmd);
     };

    class command_handler {
    public:
        virtual void handle(const std::shared_ptr<tcp_connection>& emitter, const command& cmd) = 0;
    };
}

