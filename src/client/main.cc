//
// Created by benoit on 18/09/05.
//

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <iostream>
#include <thread>
#include "tcp_client.h"
#include "common/command.h"
#include "ui.h"

// ncurse stuff
#include <panel.h>
#include "common/event_queue.h"

void run_network(boost::asio::io_service *io_service) {
    io_service->run();
}

int main()
{
    quizzbot::event_queue<quizzbot::command> queue;
    boost::asio::io_service io_service;
    quizzbot::tcp_client c(io_service, "127.0.0.1", "7778", &queue);
    std::thread network_thread(run_network, &io_service);

    quizzbot::ui gui{&c};
    gui.update();


    while(gui.should_run())
    {
        std::queue<quizzbot::command> current_commands;
        if (queue.size() != 0) {
            current_commands = queue.empty();
        }

        while (!current_commands.empty()) {
            auto cmd = current_commands.front();
            current_commands.pop();
            gui.write_to_chat("ME", cmd.content());
        }

        gui.handle_keyboard();
        gui.update();
    }

    io_service.stop();
    network_thread.join();

}