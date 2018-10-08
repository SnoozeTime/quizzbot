//
// Created by benoit on 18/09/10.
//

#pragma once
#include <vector>
#include <cstdint>

namespace quizzbot {
    using Packet = std::vector<std::uint8_t>;
    using Iter = Packet::iterator;

    enum class parse_code {
        NOT_ENOUGH,
        PARSE_OK,
        BAD_CHECKSUM,
        BLURP,
    };

    template<typename... Ts> inline void UNUSED(Ts&&...) {}
}