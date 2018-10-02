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
    quizzbot::event_queue<quizzbot::Message> queue;
    boost::asio::io_service io_service;
    quizzbot::tcp_client c(io_service, "127.0.0.1", "7778", &queue);
    std::thread network_thread(run_network, &io_service);

    // Ask the name before starting the GUI.
    std::cout << "Input your name\n";
    std::string name;
    std::cin >> name;

    quizzbot::Message join_msg;
    join_msg.set_data(std::make_unique<quizzbot::JoinMessage>(name));
    c.send(join_msg);

    // wait for message.
    bool found = false;
    while (!found) {
        std::queue<quizzbot::Message> current_commands;
        if (queue.size() != 0) {
            current_commands = queue.empty();
        }

        while (!current_commands.empty()) {
            auto cmd = current_commands.front();
            current_commands.pop();

            if (cmd.message_type() == quizzbot::MessageType::JOIN_ACK) {
                found = true;
                break;
            } else if (cmd.message_type() == quizzbot::MessageType::JOIN_NACK) {
                std::cerr << "ERROR" << std::endl;
                return 1;
            }
        }
    }

    c.set_name(name);
    quizzbot::ui gui{&c};
    gui.update();

    while(gui.should_run())
    {
        std::queue<quizzbot::Message> current_commands;
        if (queue.size() != 0) {
            current_commands = queue.empty();
        }

        while (!current_commands.empty()) {
            auto cmd = current_commands.front();
            current_commands.pop();

            if (cmd.message_type() == quizzbot::MessageType::MESSAGE) {
                auto content = dynamic_cast<const quizzbot::MessageMessage*>(cmd.data());
                assert (content);
                gui.write_to_chat(content->from() + ": " + content->msg());
            }
        }

        gui.handle_keyboard();
        gui.update();
    }

    io_service.stop();
    network_thread.join();

}