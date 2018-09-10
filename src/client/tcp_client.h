//
// Created by benoit on 18/09/06.
//

#ifndef QUIZZBOT_TCP_CLIENT_H
#define QUIZZBOT_TCP_CLIENT_H

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <iostream>
#include "common/protocol.h"

namespace quizzbot {

    class tcp_client {
    public:
        tcp_client(boost::asio::io_service &io_service,
                   const std::string &server, const std::string &port);

        void send(const std::string& msg);
    private:
        void begin_read();
        void handle_resolve(const boost::system::error_code &err,
                            boost::asio::ip::tcp::resolver::iterator endpoint_iterator);
        void handle_connect(const boost::system::error_code &err,
                            boost::asio::ip::tcp::resolver::iterator endpoint_iterator);
        void handle_read();

        boost::asio::ip::tcp::resolver resolver_;
        boost::asio::ip::tcp::socket socket_;

        Packet input_buf_ = Packet(512);
        Packet aggr_packet_;

        naive_protocol protocol_;
    };
}

#endif //QUIZZBOT_TCP_CLIENT_H
