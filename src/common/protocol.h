//
// Created by benoit on 18/09/08.
//

#ifndef QUIZZBOT_PARSER_H
#define QUIZZBOT_PARSER_H

#include <experimental/optional>
#include <sstream>
#include <iomanip>
#include "command.h"

namespace quizzbot {

    /**
     * Calculate the checksum
     */
    class checksum_parser {
    public:
        parse_code read(const Packet& msg_bytes_, Iter& input_iter, size_t len);
    };

    /**
     * Just parse the given size.
     */
    template<class Msg, class T>
    class content_parser {
    public:
        parse_code read(Msg& msg, Iter& input_iter, size_t to_read, size_t remaining);
    private:
        T cmd_protocol_;
        checksum_parser next_parser_;
    };

    /**
     * Parse the size (length of 2) for a maximum of 255 characters for the content length
     */
    template<class Msg, class T>
    class size_parser {
    public:
        parse_code read(Msg& msg, Iter& input_iter, size_t len);

    private:
        content_parser<Msg, T> next_parser;
    };

    /**
     * Parse the first block
     */
    template<class Msg, class T>
    class sync_parser {
    public:
        parse_code read(Msg& msg, Iter& input_iter, size_t len);

    private:
        size_parser<Msg, T> next_parser;
    };


    template<class Msg, class T>
    class protocol {
    public:

        /**
         * Will return the msg in a transport packet
         *
         * @param msg
         * @return
         */
        Packet pack(Msg& msg);

        /**
         * Parse a list of bytes into a message.
         * Will remove the message once it has been parsed
         *
         * @param packet
         * @return maybe a message.
         */
        std::experimental::optional<Msg> parse(Packet& packet);

    private:
        sync_parser<Msg, T> parser_;
        T command_protocol_;
    };

    using naive_protocol = protocol<command, naive_command_protocol>;

    static int compute_cc(const Packet& content) {
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

        return parse_code::OK;
    }

    template<class Msg, class T>
    parse_code content_parser<Msg, T>::read(Msg& msg, Iter& input_iter, size_t to_read, size_t remaining) {
        if (remaining < to_read) {
            return parse_code::NOT_ENOUGH;
        }

        Packet msg_bytes(input_iter, input_iter+to_read);
        cmd_protocol_.parse(msg, msg_bytes);
        input_iter += to_read;
        remaining -= to_read;

        return next_parser_.read(msg_bytes, input_iter, remaining);
    }

    template<class Msg, class T>
    parse_code size_parser<Msg, T>::read(Msg& msg, Iter& input_iter, size_t len) {

        if (len < 2) {
            return parse_code::NOT_ENOUGH;
        }

        auto candidate_size = std::string(input_iter, input_iter+2);
        input_iter += 2;
        len -= 2;


        std::istringstream iss(candidate_size);
        size_t size;
        iss >> std::hex >> size;

        if (iss.fail()) {
            return parse_code::BLURP;
        }

        return next_parser.read(msg, input_iter, size, len);
    }

    template<class Msg, class T>
    parse_code sync_parser<Msg, T>::read(Msg& msg, Iter& input_iter, size_t len) {

        if (len < 2) {
            return parse_code::NOT_ENOUGH;
        }

        auto candidate = std::string(input_iter, input_iter+2);
        if (candidate != "\x01\x02") {
            return parse_code::BLURP;
        }

        input_iter += 2;
        len -= 2;

        return next_parser.read(msg, input_iter, len);
    }

    template<class Msg, class T>
    std::experimental::optional<Msg> protocol<Msg, T>::parse(std::vector<std::uint8_t>& packet) {
        Msg msg;
        while (!packet.empty()) {

            auto iter = packet.begin();
            auto code = parser_.read(msg, iter, packet.size());

            if (code == parse_code::NOT_ENOUGH) {
                return std::experimental::nullopt;
            } else if (code == parse_code::OK) {
                packet.erase(packet.begin(), iter);
                return std::experimental::make_optional(msg);
            }

            // Didn't find a message, we remove the first character and
            // repeat.
            packet.erase(packet.begin());
        }

        return std::experimental::nullopt;
    }

    template<class Msg, class T>
    Packet protocol<Msg, T>::pack(Msg &msg) {

        Packet msg_content = command_protocol_.pack(msg);

        // 2 for sync, 2 for size, 2 for checksum
        size_t packet_size = 2+2+msg_content.size()+2;
        Packet packet;
        packet.reserve(packet_size);

        packet.push_back('\01');
        packet.push_back('\02');
        std::stringstream ss;
        ss <<  std::setfill ('0') << std::setw(2) << std::hex << msg_content.size();
        packet.push_back(static_cast<uint8_t>(ss.str().at(0)));
        packet.push_back(static_cast<uint8_t>(ss.str().at(1)));
        packet.insert(packet.end(), msg_content.begin(), msg_content.end());

        ss.str("");
        ss <<  std::setfill ('0') << std::setw(2) << std::hex << compute_cc(msg_content);
        std::string cc = ss.str();
        packet.push_back(static_cast<uint8_t>(cc[0]));
        packet.push_back(static_cast<uint8_t>(cc[1]));
        return packet;
    }
}


#endif //QUIZZBOT_PARSER_H
