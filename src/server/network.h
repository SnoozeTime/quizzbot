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

namespace quizzbot {

    class chat_room;

    class tcp_connection : public std::enable_shared_from_this<tcp_connection> {
    public:
        using pointer = std::shared_ptr<tcp_connection>;

        static pointer create_connection(boost::asio::io_service &io, chat_room *room) {
            return pointer(new tcp_connection(io, room));
        }

        boost::asio::ip::tcp::socket& socket() {
            return socket_;
        }

        void welcome();

        void send_message(const std::string& msg);

    private:
        explicit tcp_connection(boost::asio::io_service &io, chat_room *room):
                socket_(io),
                room_(room) {
        }

        void begin_read();
        void handle_receive();

        boost::asio::ip::tcp::socket socket_;

        // Handlers are installed by the server and are long-lived. If a connection has
        // a dangling pointer to a handler that is clearly a logic error and it should
        // fail hard.
        std::vector<command_handler*> handlers_;

        chat_room *room_;
        std::vector<uint8_t> input_buf_ = std::vector<uint8_t>(512);
        std::vector<uint8_t> acc_packet_;

        naive_protocol protocol_;
    };

    class chat_room: public command_handler {
    public:

        void handle(const tcp_connection::pointer& emitter, const command& cmd) override;

        // Add a participant to the chat room.
        void add_participant(const tcp_connection::pointer& participant);

        // Send a message to all participants except the emitter.
        void transmit_message(const tcp_connection::pointer& emitter, const std::string& message);

        // Send a message to all participants.
        void broadcast(const std::string& message);
    private:
        std::vector<tcp_connection::pointer> participants_;
    };


    class tcp_server {
    public:
        explicit tcp_server(boost::asio::io_service& io, int port):
                acceptor_(io, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port)) {
            LOG_INFO << "Listening on port " << port;
            start_accept();
        }

    private:

        // Waiting for a connection
        void start_accept();

        void handle_new_connection(tcp_connection::pointer connection);

        chat_room room_;
        boost::asio::ip::tcp::acceptor acceptor_;
    };
}

#endif //QUIZZBOT_NETWORK_H
