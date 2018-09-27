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

    // Ask the name before starting the GUI.
    std::cout << "Input your name\n";
    std::string name;
    std::cin >> name;
    c.send(quizzbot::command{quizzbot::command::command_type::JOIN_REQUEST, name});

    // wait for message.
    bool found = false;
    while (!found) {
        std::queue<quizzbot::command> current_commands;
        if (queue.size() != 0) {
            current_commands = queue.empty();
        }

        while (!current_commands.empty()) {
            auto cmd = current_commands.front();
            current_commands.pop();

            if (cmd.type() == quizzbot::command::command_type::JOIN_ACK) {
                found = true;
                break;
            } else if (cmd.type() == quizzbot::command::command_type::JOIN_NACK) {
                std::cerr << cmd.content() << std::endl;
                return 1;
            }
        }
    }

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
            gui.write_to_chat(cmd.content());
        }

        gui.handle_keyboard();
        gui.update();
    }

    io_service.stop();
    network_thread.join();

}