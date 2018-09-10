//
// Created by benoit on 18/09/05.
//

#include "network.h"
#include <iostream>
#include "common/command.h"
#include <algorithm>

namespace quizzbot {

// ------------------------------------------------------------------------------------------------------------------
//  TCP CONNECTION
// ------------------------------------------------------------------------------------------------------------------

void tcp_connection::welcome() {

    command msg(command::command_type::MESSAGE, "hello");
    boost::asio::async_write(socket_, boost::asio::buffer(protocol_.pack(msg)),
                             [] (const boost::system::error_code& /* error */, size_t /*bytes_transferred*/) {

                             });
    begin_read();
}
void tcp_connection::begin_read() {

    socket_.async_read_some(boost::asio::buffer(input_buf_), [this] (const boost::system::error_code& error,
            size_t bytes_received) {
        if (!error) {
            // hum that is inefficient. Figure out a way to read directly from the input buffer...
            std::cout << "Received " << bytes_received << "bytes\n";
            acc_packet_.insert(acc_packet_.end(), input_buf_.begin(), input_buf_.begin()+bytes_received);
            handle_receive();
        } else {
            std::cerr << error.message() << std::endl;
        }
    });
}

void tcp_connection::handle_receive() {

    auto maybe_msg = protocol_.parse(acc_packet_);
    if (maybe_msg) {
        auto content = maybe_msg.value().content();
        std::string msg(content.begin(), content.end());
        std::cout << "Received message > " << std::string(content.begin(), content.end()) << std::endl;
        room_->transmit_message(shared_from_this(), msg);
    }

    begin_read();
}

void tcp_connection::send_message(const std::string& msg_content) {

    command msg(command::command_type::MESSAGE, msg_content);
    boost::asio::async_write(socket_, boost::asio::buffer(protocol_.pack(msg)),
                             [] (const boost::system::error_code& /* error */, size_t /*bytes_transferred*/) {

                             });
}

// ------------------------------------------------------------------------------------------------------------------
//  CHAT ROOM
// ------------------------------------------------------------------------------------------------------------------

void chat_room::handle(const tcp_connection::pointer& emitter, const quizzbot::command &cmd) {
    switch(cmd.type()) {
        case command::command_type::MESSAGE:
            transmit_message(emitter, cmd.content());
        default:
            // This handle won't do anything for other commands
            break;
    }
}

void chat_room::add_participant(const tcp_connection::pointer& participant) {
    participants_.push_back(participant);
}

void chat_room::transmit_message(const tcp_connection::pointer& emitter, const std::string& message) {
    for (const auto& participant: participants_) {
        if (participant != emitter) {
            participant->send_message(message);
        }
    }
}

// ------------------------------------------------------------------------------------------------------------------
//  TCP SERVER
// ------------------------------------------------------------------------------------------------------------------
// Waiting for a connection
void tcp_server::start_accept() {
    tcp_connection::pointer connection = tcp_connection::create_connection(acceptor_.get_io_service(),
                                                                           &room_);

    acceptor_.async_accept(connection->socket(), [connection, this] (const boost::system::error_code& error) {
        if (!error) {
            LOG_INFO << "Received new connection from " << connection->socket().remote_endpoint().address();
            handle_new_connection(connection);
        } else {
            LOG_INFO << error.message() << std::endl;
        }
    });
}

void tcp_server::handle_new_connection(tcp_connection::pointer connection) {
    room_.add_participant(connection->shared_from_this());
    connection->welcome();
    start_accept();
}

}