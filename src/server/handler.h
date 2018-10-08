#pragma once

#include "tcp_connection.h"
#include "common/message.h"
#include "common/event_queue.h"
#include <experimental/optional>
#include "common/messages/JoinAckMessage.h"
#include "common/messages/JoinNackMessage.h"
#include "common/messages/JoinMessage.h"


namespace quizzbot {

    template <typename Msg>
    struct GameEventBase {
        GameEventBase(std::string name, Message msg):
            name(std::move(name)),
            msg(std::move(msg)) {}
        GameEventBase(GameEventBase&& other) noexcept:
                name(std::move(other.name)),
                msg(std::move(other.msg)) {

        }
        GameEventBase& operator=(GameEventBase&& other) {
            if (this == &other) {
                return *this;
            }

            name = std::move(other.name);
            msg = std::move(other.msg);

            return *this;
        }

        std::string name;
        Msg msg;
    };

    template <typename Msg, typename MsgParser>
    class Handler {
    public:
        virtual ~Handler() {}
        virtual void handle(const std::shared_ptr<TcpConnectionBase<Msg, MsgParser>>& emitter, const Msg& cmd) = 0;
    };

    template <typename Msg, typename MsgParser>
    class ChatRoomBase: public Handler<Msg, MsgParser> {
    public:


        void handle(const std::shared_ptr<TcpConnectionBase<Msg, MsgParser>>& emitter, const Msg& cmd) override {
            switch (cmd.message_type()) {
                case MessageType::MESSAGE:
                    transmit_message(emitter, cmd);
                    break;
                default:
                    // Do nothing
                    break;
            }
        }

        // Add a participant to the chat room.
        void add_participant(const std::shared_ptr<TcpConnectionBase<Msg, MsgParser>>& participant) {
            participants_.push_back(participant);
        }

        // Send a message to all participants except the emitter.
        void transmit_message(const std::shared_ptr<TcpConnectionBase<Msg, MsgParser>>& emitter, const Msg& message) {
            for (const auto& participant: participants_) {
                if (participant != emitter) {
                    participant->send_command(message);
                }
            }
        }

        void send_to(const std::string& target, const Msg& message) {
            auto target_conn = find_by_name(target);
            if (target_conn) {
                target_conn.value()->send_command(message);
            }
        }

        // Send a message to all participants.
        void broadcast(const Msg& message) {
            for (const auto& participant: participants_) {
                participant->send_command(message);
            }
        }

        /// This will try to find a player by name. return empty optional if cannot find.
        ///
        /// \param name
        /// \return
        std::experimental::optional<TcpConnectionBase<Msg, MsgParser>*> find_by_name(const std::string& name) {
            if (name.empty()) {
                return std::experimental::nullopt;
            }

            auto it = std::find_if(participants_.begin(), participants_.end(), [&name] (const std::shared_ptr<TcpConnectionBase<Msg, MsgParser>>& p) {
                return p->name() == name;
            });

            if (it != participants_.end()) {
                // miam
                return std::experimental::optional<TcpConnectionBase<Msg, MsgParser>*>{(*it).get()};
            }

            return std::experimental::nullopt;
        }
    private:

        std::vector<std::shared_ptr<TcpConnectionBase<Msg, MsgParser>>> participants_;
    };

    template <typename Msg, typename MsgParser>
    class GameHandlerBase: public Handler<Msg, MsgParser>{
    public:
        explicit GameHandlerBase(event_queue<GameEventBase<Msg>> *queue): queue_(queue) {}
        void handle(const std::shared_ptr<TcpConnectionBase<Msg, MsgParser>>& emitter, const Msg& cmd) override {
            LOG_INFO << "GAME HANDLER HANDLE THINGS";
            //Msg copy{cmd};

            queue_->push(GameEventBase<Msg>{emitter->name(), cmd});
        }
    private:
        // Event queue (thread safe) that is shared between the main thread and the network threads.
        // Network threads are the producers and will push command to the queue. Main thread is the
        // only consumer.
        event_queue<GameEventBase<Msg>>* queue_;
    };

    template <typename Msg, typename MsgParser>
    class JoinHandlerBase: public Handler<Msg, MsgParser>{
    public:
        JoinHandlerBase(ChatRoomBase<Msg, MsgParser>* room):
            room_(room) {}

        void handle(const std::shared_ptr<TcpConnectionBase<Msg, MsgParser>>& /*emitter*/, const Msg& /*msg*/) override {
            // Need to be specialized.
        }
    private:

        ChatRoomBase<Msg, MsgParser>* room_;
    };

    template <>
    class JoinHandlerBase<Message, message_protocol>: public Handler<Message, message_protocol> {
    public:
        JoinHandlerBase(ChatRoomBase<Message, message_protocol>* room):
                room_(room) {}

        void handle(const std::shared_ptr<TcpConnectionBase<Message, message_protocol>>& emitter, const Message& msg) override {
            if (msg.message_type() != MessageType::JOIN_REQUEST) {
                return;
            }

            auto data = dynamic_cast<const JoinMessage*>(msg.data());
            assert (data);

            auto candidate_name = data->name();
            if (candidate_name.empty()) {
                Message nack;
                nack.set_data(std::make_unique<JoinNackMessage>("Name cannot be empty."));
                emitter->send_command(nack);
                return;
            }

            auto maybe_existing = room_->find_by_name(candidate_name);
            if (maybe_existing) {
                Message nack;
                nack.set_data(std::make_unique<JoinNackMessage>("Name already exists."));
                emitter->send_command(nack);
                return;
            }

            emitter->set_name(candidate_name);

            Message ack;
            ack.set_data(std::make_unique<JoinAckMessage>());
            emitter->send_command(ack);
        }
    private:

        ChatRoomBase<Message, message_protocol>* room_;
    };

    using ChatRoom = ChatRoomBase<Message, message_protocol>;
    using GameHandler = GameHandlerBase<Message, message_protocol>;
    using JoinHandler = JoinHandlerBase<Message, message_protocol>;
    using GameEvent = GameEventBase<Message>;
}