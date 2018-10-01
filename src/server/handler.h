//
// Created by benoit on 18/10/02.
//

#ifndef QUIZZBOT_HANDLER_H
#define QUIZZBOT_HANDLER_H

#include "tcp_connection.h"
#include "common/message.h"
#include <experimental/optional>

namespace quizzbot {

    template <typename Msg, typename MsgParser>
    class Handler {
    public:
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
        explicit GameHandlerBase(event_queue<Msg> *queue): queue_(queue) {}
        void handle(const TcpConnection::pointer& /*emitter*/, const Msg& cmd) override {
            LOG_INFO << "GAME HANDLER HANDLE THINGS";
            queue_->push(cmd);
        }
    private:
        // Event queue (thread safe) that is shared between the main thread and the network threads.
        // Network threads are the producers and will push command to the queue. Main thread is the
        // only consumer.
        event_queue<Message>* queue_;
    };

    using ChatRoom = ChatRoomBase<Message, MessageProtocol>;
    using GameHandler = GameHandlerBase<Message, MessageProtocol>;
}


#endif //QUIZZBOT_HANDLER_H
