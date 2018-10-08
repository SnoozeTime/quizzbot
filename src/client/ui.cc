//
// Created by benoit on 18/09/22.
//

#include <string>
#include <sstream>
#include <iostream>
#include <cstring>
#include <assert.h>
#include "ui.h"
#include "common/messages/MessageMessage.h"

#include "tcp_client.h"
using namespace quizzbot;

ui::ui(tcp_client *client):
    client_(client) {
    initscr();
    cbreak();
    noecho();
    curs_set(0);
    nodelay(stdscr, true);
    keypad(stdscr, true);

    int y2, x2;
    getmaxyx(stdscr, y2, x2);
    int x1 = 2 * x2 /3;
    int y1 = y2 / 2;

    /* Create windows for the panels */
    // The game panel
    my_wins[0] = newwin(y1, x1 - 1, 1, 1);

    // The chat panel
    my_wins[1] = newwin(y2 - y1 - 5, x2 - 1, y1 + 1, 1);
    scrollok(my_wins[1], true);

    // The participant panel
    my_wins[2] = newwin(y1, x2 - x1 - 2, 1, x1 + 1);

    // input panel
    my_wins[3] = newwin(4, x2 - 1, y2 - 4, 1);

    nodelay(my_wins[0], true);
    nodelay(my_wins[1], true);
    nodelay(my_wins[2], true);
    nodelay(my_wins[3], true);
    /*
     * Create borders around the windows so that you can see the effect
     * of panels
     */
    for (int i = 0; i < 4; ++i) {
        box(my_wins[i], 0, 0);
    }

    /* Attach a panel to each window */ 	/* Order is bottom up */
    my_panels[0] = new_panel(my_wins[0]); 	/* Push 0, order: stdscr-0 */
    my_panels[1] = new_panel(my_wins[1]); 	/* Push 1, order: stdscr-0-1 */
    my_panels[2] = new_panel(my_wins[2]); 	/* Push 2, order: stdscr-0-1-2 */
    my_panels[3] = new_panel(my_wins[3]); 	/* Push 3, order: stdscr-0-1-2-3 */
    top_panel_ = my_panels[3];

    set_panel_userptr(my_panels[0], my_panels[1]);
    set_panel_userptr(my_panels[1], my_panels[2]);
    set_panel_userptr(my_panels[2], my_panels[3]);
    set_panel_userptr(my_panels[3], my_panels[0]);

    // Now add the form;
    field[1] = nullptr;
    field[0] = new_field(2, x1 - 3, y2-3, 2, 0, 0);
    //set_field_back(field[0], A_UNDERLINE); 	/* Print a line for the option 	*/
    field_opts_off(field[0], O_AUTOSKIP);  	/* Don't go to next field when this */
    form_ = new_form(field);
    set_form_win(form_, my_wins[1]);
    post_form(form_);
    refresh();
}

void ui::handle_keyboard() {
    int ch = getch();
    switch(ch)
    {
        // escape
        case 27:
            should_run_ = false;
            break;
        //enter.
        case 10:
            send_msg();
            break;
        // TAB
        case 9:
            switch_panel();
            break;
        case KEY_BACKSPACE:
            //form_driver(form_, REQ_END_FIELD);
            form_driver(form_, REQ_DEL_PREV);
            break;
        default:
            form_driver(form_, ch);
            break;
    }

    refresh();
}

void ui::send_msg() {

    form_driver(form_, REQ_VALIDATION);
    auto buf = field_buffer(field[0], 0);
    if (buf != nullptr) {
        write_to_chat("me:" + std::string(buf));
        client_->send(quizzbot::Message{std::make_unique<MessageMessage>(client_->name(), std::string{buf})});
    } else {
        write_to_chat(strerror(errno));
    }

    // Clear the field so that we can send a new message.
    form_driver(form_, REQ_CLR_FIELD);
}

void ui::update() {
    update_panels();
    doupdate();
}

ui::~ui() {
    endwin();
}

void ui::write_to_chat(std::string msg) {

    // Trim whitespaces at the end of the msg. For example when we get a message from the input field,
    // it will pad the buffer with empty charcters..
    auto last_index = msg.find_last_not_of(' ');
    wprintw(my_wins[1], "%s\n", msg.substr(0, last_index+1).c_str());
    //mvwprintw(my_wins[1], current_chat_line, 2, "%s", ss.str().c_str());
    refresh();
}

void ui::switch_panel() {
    auto next_panel = const_cast<PANEL*>(static_cast<const PANEL*>(panel_userptr(top_panel_)));
    top_panel_ = next_panel;
    top_panel(top_panel_);
}

