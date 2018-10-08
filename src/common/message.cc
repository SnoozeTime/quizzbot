#include "message.h"


#include "messages/MessageMessage.h"
#include "messages/AnswerMessage.h"
#include "messages/JoinMessage.h"
#include "messages/JoinAckMessage.h"
#include "messages/JoinNackMessage.h"
#include "messages/ErrorMessage.h"

namespace quizzbot {
  std::unique_ptr<MessageData> create_data(const MessageType& type) {
        switch (type) {
	  
	case MessageType::MESSAGE:
	return std::make_unique<MessageMessage>();
	case MessageType::ANSWER:
	return std::make_unique<AnswerMessage>();
	case MessageType::JOIN_REQUEST:
	return std::make_unique<JoinMessage>();
	case MessageType::JOIN_ACK:
	return std::make_unique<JoinAckMessage>();
	case MessageType::JOIN_NACK:
	return std::make_unique<JoinNackMessage>();
	case MessageType::ERROR:
	return std::make_unique<ErrorMessage>();
            default:
                assert(false);
        }
  }
}