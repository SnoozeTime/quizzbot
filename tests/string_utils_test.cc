//
// Created by benoit on 18/10/08.
//

#include "common/string_utils.h"
#include <gtest/gtest.h>

TEST(trim, trim_left) {
    ASSERT_STREQ("", quizzbot::trim_left("").c_str());
    ASSERT_STREQ("Bonjour", quizzbot::trim_left(" Bonjour").c_str());
    ASSERT_STREQ("Bonjour", quizzbot::trim_left("  Bonjour").c_str());
    ASSERT_STREQ("\tBonjour", quizzbot::trim_left("\tBonjour").c_str());
}

TEST(trim, trim_right) {
    ASSERT_STREQ("", quizzbot::trim_right("").c_str());
    ASSERT_STREQ("Bonjour", quizzbot::trim_right("Bonjour  ").c_str());
    ASSERT_STREQ("Bonjour", quizzbot::trim_right("Bonjour ").c_str());
    ASSERT_STREQ("\tBonjour\n \t", quizzbot::trim_right("\tBonjour\n \t").c_str());
}

TEST(trim, trim) {
    ASSERT_STREQ("", quizzbot::trim("").c_str());
    ASSERT_STREQ("Bonjour", quizzbot::trim(" Bonjour  ").c_str());
    ASSERT_STREQ("Bonjour", quizzbot::trim("    Bonjour ").c_str());
    ASSERT_STREQ("\tBonjour\n \t", quizzbot::trim("\tBonjour\n \t").c_str());
}