//
// Created by benoit on 18/09/09.
//

#include "game.h"

using namespace quizzbot;

std::atomic_bool should_run{true};

game::game():
    io_(),
    io_work_(io_),
    io_thread_([this] { io_.run(); }){

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
    server_ = std::make_unique<tcp_server>(io_, 7777);
    while (should_run) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    io_thread_.join();
}