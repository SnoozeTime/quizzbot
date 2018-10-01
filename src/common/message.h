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

        Message(Message& other) {
            if (other.message_type() != MessageType::NONE) {
                type_ = other.message_type();
                // TODO deep copy message. Maybe serialize before queue and deserialize after queue instead...
            }
        }

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
            return data_.get();
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

    class JoinMessage: public MessageData {
    public:

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

        MessageType message_type() override {
            return MessageType::JOIN_ACK;
        }

        // empty message.
        void unpack(std::string& /*ss*/) override {}
        void pack(std::stringstream& /*ss*/) override {}
    };

    class JoinNackMessage: public MessageData {
    public:

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

    std::unique_ptr<MessageData> create_data(const MessageType& type) {
        std::cout << (int) type << std::endl;
        switch (type) {
            case MessageType::MESSAGE:
                return std::make_unique<MessageMessage>();
            case MessageType::JOIN_REQUEST:
                return std::make_unique<JoinMessage>();
            default:
                assert(false);
        }
    }

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
