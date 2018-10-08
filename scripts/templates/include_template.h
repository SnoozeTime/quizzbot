#pragma once

#include <stddef.h>
#include "quizzbot_types.h"
#include <sstream>
#include <msgpack.hpp>
#include <vector>
#include <iostream>

namespace quizzbot {


  enum class MessageType {
    {% for enum_name in enums %}
	  {{ enum_name}},{% endfor %}
	  NONE,
    };

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
