#pragma once

#include <cstddef>
#include <cstdint>

namespace keyviz
{
struct KeyBinding
{
    const char* label;
    std::uint32_t keyCode;
    float widthScale;
};

struct KeyRow
{
    const KeyBinding* keys;
    std::size_t keyCount;
};

struct KeyRowSet
{
    const KeyRow* rows = nullptr;
    std::size_t count = 0;
};

int GetLayoutPresetCount();
const char* const* GetLayoutPresetLabels();
KeyRowSet GetRowsForPreset(int presetIndex);
void ApplyLayoutPresetDefaults(int presetIndex, float& layoutScale);
} // namespace keyviz
