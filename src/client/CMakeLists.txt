add_executable(client main.cc tcp_client.cc ui.cc)
target_link_libraries(client quizzbot ${Boost_LIBRARIES} pthread ncurses panel form)
