//
// Created by benoit on 18/09/12.
//

#ifndef QUIZZBOT_PLAYER_H
#define QUIZZBOT_PLAYER_H

#include <string>

namespace quizzbot {

    enum class player_state {
        NAMELESS,
        READY
    };

    class player {
    public:


        player(): state_(player_state::NAMELESS), name_() {}
        player_state state() { return state_; }

    private:
        enum player_state state_;
        std::string name_;
    };
}


#endif //QUIZZBOT_PLAYER_H
