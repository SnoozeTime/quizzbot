//
// Created by benoit on 18/09/05.
//

#ifndef QUIZZBOT_NETWORK_H
#define QUIZZBOT_NETWORK_H

#include <memory>
#include <boost/asio.hpp>
#include "common/protocol.h"
#include "common/log.h"
#include "common/command.h"
#include "common/event_queue.h"

#include "handler.h"
#include "tcp_connection.h"

namespace quizzbot {

    class tcp_server {
    public:
        explicit tcp_server(boost::asio::io_service& io, unsigned short port, ChatRoom *room, event_queue<GameEvent> *queue):
                room_{room},
                game_handler_(queue),
                join_handler_{room},
                acceptor_{io, boost::asio::ip::tcp::endpoint{boost::asio::ip::tcp::v4(), port}} {
            LOG_INFO << "Listening on port " << port;
            start_accept();
        }

    private:

        // Waiting for a connection
        void start_accept();

        void handle_new_connection(const TcpConnection::pointer& connection);

        // Order of construction is important. Join handler depends on chat room
        ChatRoom *room_;
        GameHandler game_handler_;
        JoinHandler  join_handler_;
        boost::asio::ip::tcp::acceptor acceptor_;
    };
}

#endif //QUIZZBOT_NETWORK_H
