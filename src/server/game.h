//
// Created by benoit on 18/09/09.
//

#ifndef QUIZZBOT_GAME_H
#define QUIZZBOT_GAME_H

#include <thread>
#include <boost/asio.hpp>
#include <memory>
#include "network.h"
#include "common/event_queue.h"
#include "quizz_system.h"

namespace quizzbot {

class game {

public:

    enum class game_state {
        WAITING_FOR_ANSWER, // Wait for answer from the clients. Will finish after timeout.
        INITIALIZING, // Get a new equation and will send it to all clients.
        CLOSING_CURRENT_SESSION, // if somebody has correct answer, add a point
    };
    game();

    void loop();

private:

    boost::asio::io_service io_;
    // the io service work is used to keep the io loop busy until
    // we initialize the server.
    boost::asio::io_service::work io_work_;
    std::thread io_thread_;

    // Why unique_ptr? I want to control the initialization
    std::unique_ptr<ChatRoom> participants_;
    std::unique_ptr<tcp_server> server_;
    std::unique_ptr<QuizzSystem> quizz_system_;
};

}

#endif //QUIZZBOT_GAME_H
