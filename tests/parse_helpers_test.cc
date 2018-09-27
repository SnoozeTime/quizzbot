//
// Created by benoit on 18/09/28.
//

#include <gtest/gtest.h>
#include <common/parse_helpers.h>

TEST(parse_size_t, when_ok) {

}

TEST(parse_size_t, when_iter_too_short) {

}

TEST(parse_size_t, when_size_too_long) {
    std::vector<uint8_t> expected_packet = {'F', 'F', 'n', '9', '2', 'c'};

    size_t size_len = 3; // "FFn" shouldn't be parsed to a length.

    size_t size;
    auto iter = expected_packet.begin();
    auto code = quizzbot::parse_hex_size(iter, expected_packet.size(), size, size_len);

    ASSERT_EQ(quizzbot::parse_code::BLURP, code);
}