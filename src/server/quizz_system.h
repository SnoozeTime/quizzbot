//
// Created by benoit on 18/10/08.
//

#ifndef QUIZZBOT_QUIZZ_SYSTEM_H
#define QUIZZBOT_QUIZZ_SYSTEM_H

#include <chrono>
#include <string>
#include <unordered_map>

namespace quizzbot {

    class game;

    class QuizzSystem {

    public:
        QuizzSystem(game* game);

        void update(std::chrono::milliseconds dt);

        void submit_answer(std::string from, std::string answer);
    private:
        game* game_;
        std::chrono::milliseconds current_session_dt_;
        std::string current_answer_;
        std::string current_question_;

        // Naive way to keep everybody's answer. Name should be unique so we can use a map.
        std::unordered_map<std::string, std::string> answers_;
        // This will choose a question and answer and send it to the clients.
        void init_session();

    };

}


#endif //QUIZZBOT_QUIZZ_SYSTEM_H
