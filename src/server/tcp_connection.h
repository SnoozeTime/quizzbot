//
// Created by benoit on 18/10/02.
//

#ifndef QUIZZBOT_TCP_CONNECTION_H
#define QUIZZBOT_TCP_CONNECTION_H

#include <memory>
#include <boost/asio.hpp>
#include "network.h"

namespace quizzbot {

    template<typename Msg, typename MsgProtocol>
    class TcpConnectionBase : public std::enable_shared_from_this<TcpConnectionBase<Msg, MsgProtocol>> {
    public:
        using pointer = std::shared_ptr<TcpConnectionBase>;

        static pointer create_connection(boost::asio::io_service &io, chat_room *room, game_handler* game) {
            return pointer(new TcpConnectionBase(io, room, game));
        }

        boost::asio::ip::tcp::socket& socket() {
            return socket_;
        }

        void welcome();
        void send_command(const Msg& cmd);

        const std::string& name() const { return name_; }

    private:

        explicit TcpConnectionBase(boost::asio::io_service &io, chat_room *room, game_handler* game);

        void begin_read();
        void handle_receive();

        /// This is when a player does not have a name yet. He shouldn't be able to interact with other people
        /// yet.
        void handle_join(const command& cmd);

        boost::asio::ip::tcp::socket socket_;

        chat_room *room_;
        // Handlers are installed by the server and are long-lived. If a connection has
        // a dangling pointer to a handler that is clearly a logic error and it should
        // fail hard.
        std::vector<command_handler*> handlers_;

        std::vector<uint8_t> input_buf_ = std::vector<uint8_t>(512);
        std::vector<uint8_t> acc_packet_;

        MsgProtocol protocol_;

        // name of the player. Empty at first. Need to be set before anything else. :)
        std::string name_;
    };

    using TcpConnection = TcpConnectionBase<Message, MessageProtocol>;

}


#endif //QUIZZBOT_TCP_CONNECTION_H
