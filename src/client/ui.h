//
// Created by benoit on 18/09/22.
//

#ifndef QUIZZBOT_UI_H
#define QUIZZBOT_UI_H

#include <panel.h>
#include <string>
#include <iostream>
#include "common/event_queue.h"
#include "line_parser.h"
#include <form.h>

namespace quizzbot {

    class tcp_client;

class ui {
public:
    ui(tcp_client *client);
    ~ui();

    void update();

    /// Will write the following message to the chat.
    /// \param msg
    void write_to_chat(std::string msg);


    void handle_keyboard();

    bool should_run() const { return should_run_; }
private:

    WINDOW *my_wins[4];
    PANEL  *my_panels[4];
    PANEL *top_panel_;
    FIELD *field[2];
    FORM *form_;

    bool should_run_ = true;

    tcp_client *client_;
    LineParser line_parser_;

    void switch_panel();
    void send_msg();
};
}

#endif //QUIZZBOT_UI_H
