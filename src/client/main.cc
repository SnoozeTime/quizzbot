//
// Created by benoit on 18/09/05.
//

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <iostream>
#include <thread>
#include "tcp_client.h"

void run_network(boost::asio::io_service *io_service) {

    io_service->run();
}

int main()
{
    boost::asio::io_service io_service;
    quizzbot::tcp_client c(io_service, "127.0.0.1", "7777");
    std::thread network_thread(run_network, &io_service);

    int i = 0;
    while (i < 5) {

        std::string msg;
        std::cin >> msg;

        c.send(msg);

        i++;
    }
}