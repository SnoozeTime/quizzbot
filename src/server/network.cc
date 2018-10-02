//
// Created by benoit on 18/09/05.
//

#include "network.h"
#include <iostream>
#include "common/command.h"
#include <algorithm>

namespace quizzbot {

// ------------------------------------------------------------------------------------------------------------------
//  TCP SERVER
// ------------------------------------------------------------------------------------------------------------------
// Waiting for a connection
void tcp_server::start_accept() {
    TcpConnection::create_connection(acceptor_.get_io_service(), &room_, &game_handler_, &join_handler_);
    auto connection = TcpConnection::create_connection(acceptor_.get_io_service(),
                                                       &room_,
                                                       &game_handler_,
                                                       &join_handler_);

    acceptor_.async_accept(connection->socket(), [connection, this] (const boost::system::error_code& error) {
        if (!error) {
            LOG_INFO << "Received new connection from " << connection->socket().remote_endpoint().address();
            handle_new_connection(connection);
        } else {
            LOG_INFO << error.message() << std::endl;
        }
    });
}

void tcp_server::handle_new_connection(const TcpConnection::pointer& connection) {
    room_.add_participant(connection->shared_from_this());
    connection->welcome();
    start_accept();
}

}