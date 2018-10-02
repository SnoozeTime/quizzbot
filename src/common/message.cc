//
// Created by benoit on 18/09/28.
//

#include "message.h"

namespace quizzbot {
    std::unique_ptr<MessageData> create_data(const MessageType& type) {
        switch (type) {
            case MessageType::MESSAGE:
                return std::make_unique<MessageMessage>();
            case MessageType::JOIN_REQUEST:
                return std::make_unique<JoinMessage>();
            case MessageType::JOIN_ACK:
                return std::make_unique<JoinAckMessage>();
            case MessageType::JOIN_NACK:
                return std::make_unique<JoinNackMessage>();
            default:
                assert(false);
        }
    }
}