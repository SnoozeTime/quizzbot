//
// Created by benoit on 18/09/09.
//

#include "game.h"
#include <iostream>
#include "handler.h"

using namespace quizzbot;

std::atomic_bool should_run{true};

game::game():
    io_(),
    io_work_(io_),
    io_thread_([this] { io_.run(); }),
    participants_{std::make_unique<ChatRoom>()} {

}

void game::loop() {

    // Install the sigint handler on io service
    boost::asio::signal_set signal{io_, SIGINT};
    io_.post([&signal, &io = this->io_] {
       signal.async_wait([&io](const boost::system::error_code& /*error*/,
                            int /*signal_number*/) {
           should_run.store(false);
           io.stop();
       });
    });

    // Start the server.
    event_queue<GameEvent> queue;
    server_ = std::make_unique<tcp_server>(io_, 7778, participants_.get(), &queue);
    quizz_system_ = std::make_unique<QuizzSystem>(this);

    while (should_run) {

        // ----------------------------------------------
        // 1. GET AND PROCESS INPUT FROM PLAYERS
        // ----------------------------------------------
        std::queue<GameEvent> current_commands;
        if (queue.size() != 0) {
            current_commands = queue.empty();
        }

        while (!current_commands.empty()) {

            auto cmd = std::move(current_commands.front());
            current_commands.pop();

            if (cmd.msg.message_type() == MessageType::ANSWER) {
                std::cout << "Got an answer from " << cmd.name << '\n';
            }
        }

        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    io_thread_.join();
}