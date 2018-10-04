
function (add_quizzbot_test test_class)
    add_executable(${test_class} ${test_class}.cc)
    target_link_libraries(${test_class} quizzbot gtest gtest_main)
endfunction()

function (add_quizzbot_client_test test_class)
    add_executable(${test_class} ${test_class}.cc)
    target_link_libraries(${test_class} quizzbot_client gtest gtest_main)
endfunction()

add_quizzbot_test(command_test)
add_quizzbot_test(parser_test)
add_quizzbot_test(event_queue_test)
add_quizzbot_client_test(line_parser_test)

add_executable(all_tests command_test.cc parser_test.cc)
target_link_libraries(all_tests quizzbot gtest gtest_main)