#include "message.h"

{% for msg in msgs %}
#include "messages/{{ msg.name }}Message.h"{% endfor %}

namespace quizzbot {
  std::unique_ptr<MessageData> create_data(const MessageType& type) {
        switch (type) {
	  {% for msg in msgs %}
	case MessageType::{{ msg.message_type }}:
	return std::make_unique<{{ msg.name}}Message>();{% endfor %}
            default:
                assert(false);
        }
  }
}
