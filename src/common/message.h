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
#include <msgpack.hpp>
#include <vector>
#include <iostream>

namespace quizzbot {

    enum class MessageType {
        ANSWER,
        MESSAGE,
        JOIN_REQUEST,
        JOIN_ACK,
        JOIN_NACK,
        ERROR,
        NONE,
    };

    class Message;
    class MessageData;
    class MessageMessage;
    class JoinMessage;
    class JoinAckMessage;
    class JoinNackMessage;

    class MessageData {
    public:
        virtual ~MessageData() = default;

        virtual MessageType message_type() = 0;
        virtual void unpack(std::string& ss) = 0;
        virtual void pack(std::stringstream& ss) = 0;
    };

    std::unique_ptr<MessageData> create_data(const MessageType& type);

    class Message {
    public:
        Message() = default;
        explicit Message(std::unique_ptr<MessageData>&& data):
            data_{std::move(data)} {
            if (data_) {
                type_ = data_->message_type();
            }
        }

        Message(const Message& other) {
            std::string packet;
            other.pack(packet);
            unpack(packet);
        }

        Message(Message&& other) noexcept = default;
        Message& operator=(Message&& other) = default;

        virtual ~Message() = default;

        MessageType message_type() const {
            if (data_) {
                return data_->message_type();
            }
            return MessageType::NONE;
        }
        void set_message_type(MessageType type) {
            type = type_;
        }

        // todo error handling when no data.
        const MessageData* data() const {
            if (data_) {
                return data_.get();
            } else {
                return nullptr;
            }
        }

        void set_data(std::unique_ptr<MessageData> data) {
            data_ = std::move(data);
            type_ = data_->message_type();
        }

        void pack(std::string& packet) const {
            // First element is the message type.
            std::stringstream ss;
            msgpack::pack(ss, (int) type_);
            data_->pack(ss);

            packet += ss.str();
        }

        void unpack(std::string& ss) {
            size_t offset = 0;
            auto oh = msgpack::unpack(ss.c_str(), ss.size(), offset);
            auto o = oh.get();
            // range check here.
            type_ = static_cast<quizzbot::MessageType>(o.as<int>());

            auto data_buf = ss.substr(offset);
            data_ = create_data(type_);
            data_->unpack(data_buf);
        }
    private:
        MessageType type_{MessageType::NONE};
        std::unique_ptr<MessageData> data_;
    };


    class MessageMessage: public MessageData {
    public:

        MessageMessage() = default;
        MessageMessage(std::string from, std::string msg): from_{std::move(from)}, msg_{std::move(msg)} {}
        MessageMessage(MessageMessage&& other) noexcept = default;
        MessageMessage& operator=(MessageMessage& other) = default;

        void set_from(const std::string& from) { from_ = from; }
        void set_msg(const std::string& msg) { msg_ = msg; }
        const std::string& from() const { return from_; }
        const std::string& msg() const { return msg_; }

        MessageType message_type() override {
            return MessageType::MESSAGE;
        }

        void unpack(std::string& ss) override {
            size_t offset = 0;
            {
                auto oh = msgpack::unpack(ss.c_str(), ss.size(), offset);
                auto o = oh.get();
                from_ = o.as<std::string>();
            }
            {
                auto oh = msgpack::unpack(ss.c_str(), ss.size(), offset);
                auto o = oh.get();
                msg_ = o.as<std::string>();
            }
        }

        void pack(std::stringstream& ss) override {
            msgpack::pack(ss, from_);
            msgpack::pack(ss, msg_);
        }

    private:
        std::string from_{};
        std::string msg_{};
    };

    class AnswerMessage: public MessageData {
    public:

        AnswerMessage() = default;
        AnswerMessage(std::string from, std::string answer): from_{std::move(from)}, answer_{std::move(answer)} {}
        AnswerMessage(AnswerMessage&& other) noexcept = default;
        AnswerMessage& operator=(AnswerMessage& other) = default;

        void set_from(const std::string& from) { from_ = from; }
        void set_answer(const std::string& answer) { answer_ = answer_; }
        const std::string& from() const { return from_; }
        const std::string& answer() const { return answer_; }

        MessageType message_type() override {
            return MessageType::ANSWER;
        }

        void unpack(std::string& ss) override {
            size_t offset = 0;
            {
                auto oh = msgpack::unpack(ss.c_str(), ss.size(), offset);
                auto o = oh.get();
                from_ = o.as<std::string>();
            }
            {
                auto oh = msgpack::unpack(ss.c_str(), ss.size(), offset);
                auto o = oh.get();
                answer_ = o.as<std::string>();
            }
        }

        void pack(std::stringstream& ss) override {
            msgpack::pack(ss, from_);
            msgpack::pack(ss, answer_);
        }

    private:
        std::string from_{};
        std::string answer_{};
    };


    class JoinMessage: public MessageData {
    public:

        JoinMessage() = default;
        explicit JoinMessage(std::string name): name_(std::move(name)) {}
        JoinMessage(JoinMessage&& other) noexcept = default;
        JoinMessage& operator=(JoinMessage& other) = default;

        void set_name(const std::string& name) { name_ = name; }
        const std::string& name() const { return name_; }

        MessageType message_type() override {
            return MessageType::JOIN_REQUEST;
        }

        void unpack(std::string& ss) override {
            size_t offset = 0;
            {
                auto oh = msgpack::unpack(ss.c_str(), ss.size(), offset);
                auto o = oh.get();
                name_ = o.as<std::string>();
            }
        }

        void pack(std::stringstream& ss) override {
            msgpack::pack(ss, name_);
        }
    private:
        std::string name_{};
    };

    class JoinAckMessage: public MessageData {
    public:
        JoinAckMessage() = default;
        JoinAckMessage(JoinAckMessage&& other) noexcept = default;
        JoinAckMessage& operator=(JoinAckMessage& other) = default;
        MessageType message_type() override {
            return MessageType::JOIN_ACK;
        }

        // empty message.
        void unpack(std::string& /*ss*/) override {}
        void pack(std::stringstream& /*ss*/) override {}
    };

    class JoinNackMessage: public MessageData {
    public:

        JoinNackMessage() = default;
        explicit JoinNackMessage(std::string error):
            error_(std::move(error)) {}
        JoinNackMessage(JoinNackMessage&& other) noexcept = default;
        JoinNackMessage& operator=(JoinNackMessage& other) = default;

        MessageType message_type() override {
            return MessageType::JOIN_NACK;
        }

        void unpack(std::string& ss) override {
            size_t offset = 0;
            {
                auto oh = msgpack::unpack(ss.c_str(), ss.size(), offset);
                auto o = oh.get();
                error_ = o.as<std::string>();
            }
        }

        void pack(std::stringstream& ss) override {
            msgpack::pack(ss, error_);
        }

        const std::string& error() const { return error_; }
        void set_error(std::string error) { error_ = std::move(error); }

    private:
        std::string error_{};
    };

    class ErrorMessage: public MessageData {
    public:

        ErrorMessage() = default;
        explicit ErrorMessage(std::string error):
                error_(std::move(error)) {}
        ErrorMessage(ErrorMessage&& other) noexcept = default;
        ErrorMessage& operator=(ErrorMessage& other) = default;

        MessageType message_type() override {
            return MessageType::ERROR;
        }

        void unpack(std::string& ss) override {
            size_t offset = 0;
            {
                auto oh = msgpack::unpack(ss.c_str(), ss.size(), offset);
                auto o = oh.get();
                error_ = o.as<std::string>();
            }
        }

        void pack(std::stringstream& ss) override {
            msgpack::pack(ss, error_);
        }

        const std::string& error() const { return error_; }
        void set_error(std::string error) { error_ = std::move(error); }

    private:
        std::string error_{};
    };


    class MessageProtocol {
    public:
        void parse(Message & msg, Packet packet) {
            std::string packet_str{packet.begin(), packet.end()};
            msg.unpack(packet_str);
        }
        Packet pack(const Message& msg) {
            std::string packet_str;
            msg.pack(packet_str);
            return Packet{packet_str.begin(), packet_str.end()};
        };
    };

}

#endif //QUIZZBOT_MESSAGE_H
