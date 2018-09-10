#include <iostream>
#include <boost/asio.hpp>
#include <memory>
#include <vector>
#include "network.h"
#include "game.h"

int main() {
    quizzbot::game game;
    game.loop();
    return 0;
}