add_library(quizzbot_client tcp_client.cc ui.cc line_parser.cc)
target_link_libraries(quizzbot_client quizzbot  ${Boost_LIBRARIES} pthread ncurses panel form)

add_executable(client main.cc tcp_client.cc ui.cc line_parser.cc)
target_link_libraries(client quizzbot_client)
