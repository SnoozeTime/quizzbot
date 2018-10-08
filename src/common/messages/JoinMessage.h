/// CLASS WAS AUTOGENERATED  - DO NOT MODIFY

#pragma once
#include <sstream>
#include <string>

#include "common/message.h"
#include "common/quizzbot_types.h"

namespace quizzbot {
class JoinMessage : public MessageData {
public:
  JoinMessage() = default;

  JoinMessage(std::string name)
      :

        name_(std::move(name)) {}

  // Default move because the types should be trivials..
  JoinMessage(JoinMessage &&other) noexcept = default;
  JoinMessage &operator=(JoinMessage &&other) = default;

  MessageType message_type() override { return MessageType::JOIN_REQUEST; }

  void unpack(std::string &ss) override {

    size_t offset = 0;

    {
      auto oh = msgpack::unpack(ss.c_str(), ss.size(), offset);
      auto o = oh.get();
      name_ = o.as<std::string>();
    }
  }

  void pack(std::stringstream &ss) override { msgpack::pack(ss, name_); }

  // Then Getters and setters

  void set_name(const std::string &name) { name_ = name; }
  const std::string &name() const { return name_; }

  // double dispatch to avoid casting.
  //  void dispatch(MessageHandler* handler) override {
  //  handler->handle(*this);
  //}

private:
  std::string name_{};
};
}