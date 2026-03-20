#pragma once

#include <cstdint>

namespace keyviz
{
struct InputEvent
{
    enum class Type
    {
        KeyDown,
        KeyUp,
    };

    Type type = Type::KeyDown;
    std::uint32_t keyCode = 0;
    std::uint64_t timestampMs = 0;
    bool isRepeat = false;
};
}
