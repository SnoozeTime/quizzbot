//
// Created by benoit on 18/09/08.
//

#include "gtest/gtest.h"
#include "common/protocol.h"

using namespace quizzbot;

TEST(parse_message, empty_packet) {
    naive_protocol p;
    std::vector<uint8_t> packet = {};
    if(p.parse(packet)) {
        FAIL();
    }
}

TEST(parse_message, wrong_checksum) {
    naive_protocol p;
    std::vector<uint8_t> packet = {'\x01', '\x02', '0', 'A', 'M', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'b', 'c'};
    if(p.parse(packet)) {
        FAIL();
    }
}

TEST(parse_message, not_enough_no_size) {
    naive_protocol p;
    std::vector<uint8_t> packet = {'\x01', '\x02', '0'};
    if(p.parse(packet)) {
        FAIL();
    }
}

TEST(parse_message, not_enough_payload) {
    naive_protocol p;
    std::vector<uint8_t> packet = {'\x01', '\x02', '0', 'A', 'M', '1', '2', '3', '4', '5'};
    if(p.parse(packet)) {
        FAIL();
    }
}

TEST(parse_message, ok_packet) {
    naive_protocol p;
    std::vector<uint8_t> packet = {'\x01', '\x02', '0', 'a', 'M', '1', '2', '3', '4', '5', '6', '7', '8', '9', '2', 'c'};
    auto maybe_msg = p.parse(packet);

    if (maybe_msg) {
        auto msg = maybe_msg.value();
        auto content = msg.content();
        ASSERT_EQ(command::command_type::MESSAGE, msg.type());
        ASSERT_STREQ("123456789", content.c_str());
    } else {
        FAIL();
    }
}

TEST(parse_message, packet_with_noise_in_front) {
    naive_protocol p;
    std::vector<uint8_t> packet = {'l', 'o', ';', '\x01', '\x02', '0', 'A', 'M', '1', '2', '3', '4', '5', '6', '7', '8', '9', '2', 'c'};
    auto maybe_msg = p.parse(packet);

    if (maybe_msg) {
        auto msg = maybe_msg.value();
        auto content = msg.content();
        ASSERT_EQ(command::command_type::MESSAGE, msg.type());
        ASSERT_STREQ("123456789", content.c_str());
    } else {
        FAIL();
    }
}

TEST(pack_msg, when_normal_packet) {
    naive_protocol p;
    std::vector<uint8_t> expected_packet = {'\x01', '\x02', '0', 'a', 'M', '1', '2', '3', '4', '5', '6', '7', '8', '9', '2', 'c'};

    command msg(command::command_type::MESSAGE, "123456789");

    auto packed = p.pack(msg);
    ASSERT_EQ(expected_packet.size(), packed.size());
    for (size_t i = 0; i < packed.size(); i++) {
        ASSERT_EQ(expected_packet[i], packed[i]);
    }
}

TEST(pack_and_parse, normal_message) {
    command msg(command::command_type::MESSAGE, "bonjour");

    naive_protocol p;
    auto packet = p.pack(msg);
    auto parsed = p.parse(packet);

    if (parsed) {
        auto parsed_msg = parsed.value();
        auto content = parsed_msg.content();
        ASSERT_EQ(command::command_type::MESSAGE, msg.type());
        ASSERT_STREQ("bonjour", content.c_str());
    } else {
        FAIL();
    }
}