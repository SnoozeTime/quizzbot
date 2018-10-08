//
// Created by benoit on 18/10/08.
//

#include "quizz_system.h"
#include "game.h"

namespace quizzbot {

    constexpr std::chrono::milliseconds SESSION_DURATION{5000};

    QuizzSystem::QuizzSystem(game* game):
        game_{game},
        current_session_dt_{0},
        current_answer_{},
        current_question_{}{
        init_session();
    }

    void QuizzSystem::submit_answer(std::string from, std::string answer) {
        answers_.emplace(from, answer);
    }

    void QuizzSystem::update(std::chrono::milliseconds dt) {
        current_session_dt_ += dt;

        if (current_session_dt_ >= SESSION_DURATION) {

            // Find the winners.
            for (const auto& answer_entries: answers_) {
                if (answer_entries.second == current_answer_) {
                    // This dude is a winner. Give him a POINT!
                }
            }

            // No need for this batch of answer anymore. Remove and then start a new session
            answers_.clear();
            init_session();
        }

        (void) game_;
        // Send back the remaining time
        // auto remaining = SESSION_DURATION - current_session_dt_;
    }

    void QuizzSystem::init_session() {
        current_question_ = "5 * 5";
        current_answer_ = "25";
    }
}