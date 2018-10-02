//
// Created by benoit on 18/09/22.
//

#include "gtest/gtest.h"
#include "common/event_queue.h"
#include <chrono>
#include <thread>

using namespace quizzbot;

TEST(event_queue, one_thread) {
    event_queue<std::string> q;
    q.push("hello");
    q.push("hello2");
    q.push("hello3");

    auto q2 = q.empty();
    size_t zero = 0;
    size_t three = 3;
    ASSERT_EQ(zero, q.size());
    ASSERT_EQ(three, q2.size());

    ASSERT_STREQ("hello", q2.front().c_str());
    q2.pop();
    ASSERT_STREQ("hello2", q2.front().c_str());
    q2.pop();
    ASSERT_STREQ("hello3", q2.front().c_str());
    q2.pop();
}

void producer(event_queue<std::string> *queue, std::string msg, int nb) {
    for (int i = 0; i < nb; i++) {
        auto copy = std::string{msg};
        queue->push(std::move(copy));
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

// This is the normal usecase. Lot of producers but only one consumer.
TEST(event_queue, bunch_of_threads) {
    event_queue<std::string> q;

    constexpr int NB_THREADS = 10;
    std::vector<std::thread> threads;
    for (int i = 0; i < NB_THREADS; i++) {
        std::stringstream ss;
        ss << "HI from " << i;
        threads.emplace_back(producer, &q, ss.str(), i+1);
    }

    size_t total_msg = 0;
    auto new_queue = q.empty();
    total_msg += new_queue.size();

    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    new_queue = q.empty();
    total_msg += new_queue.size();

    for (auto& t : threads) {
        t.join();
    }

    new_queue = q.empty();
    total_msg += new_queue.size();

    size_t expected_nb_msg = 1 + 2 + 3 + 4 + 5 + 6 + 7 + 8 + 9 + 10;
    ASSERT_EQ(expected_nb_msg, total_msg);
    auto last_queue = q.empty();
    ASSERT_TRUE(last_queue.empty());
}