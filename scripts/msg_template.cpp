#pragma once

#include <sstream>
#include <string>

class {{ msg.name }}Message: public MessageData {
public:

  {{ msg.name }}Message() = default;
  {{ msg.name }}Message({% for arg in msg.args %}{{ arg.type }} {{ arg.name }},{% endfor %});

  // Default move because the types should be trivials..
  {{ msg.name }}Message({{ msg.name }}Message&& other) noexcept = default;
  {{ msg.name }}Message& operator=({{ msg.name}}Message&& other) = default;

  void unpack(std::string& ss) override {

  }

  void pack(std::stringstream& ss) override {

  }
private:
  {% for member in msg.args %}
  {{ member.type }} {{ member.name}}_{};
  {% endfor %}
};
