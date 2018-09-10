cmake_minimum_required(VERSION 3.12)
project(quizzbot)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wall -Werror -pedantic -Wextra -Wno-missing-field-initializers")

# FOR TESTING
ADD_SUBDIRECTORY (googletest-release-1.7.0)
include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

find_package( Boost REQUIRED COMPONENTS program_options system log log_setup)
ADD_DEFINITIONS(-DBOOST_LOG_DYN_LINK)
include_directories( ${Boost_INCLUDE_DIRS} )

# -------------------------------------------
include_directories (${CMAKE_CURRENT_SOURCE_DIR}/src)

# Contain all classes without main
add_library(quizzbot src/server/network.cc
                     src/server/game.cc
                     src/common/command.cc
        src/common/protocol.cc)
target_link_libraries( quizzbot ${Boost_LIBRARIES})

# SERVER EXECUTABLE
add_executable(server src/server/main.cpp)
target_link_libraries(server quizzbot ${Boost_LIBRARIES} pthread)

# CLIENT EXECUTABLE
add_executable(client src/client/main.cc src/client/tcp_client.cc)
target_link_libraries(client quizzbot ${Boost_LIBRARIES} pthread)

# My tests !
add_subdirectory(tests)