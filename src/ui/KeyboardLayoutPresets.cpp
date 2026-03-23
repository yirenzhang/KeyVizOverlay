#include "KeyboardLayoutPresets.h"

#include <array>

#include <windows.h>

namespace keyviz
{
namespace
{
constexpr KeyBinding kTopRowKeys[] =
{
    { "Tab", VK_TAB, 1.10f },
    { "Q", 'Q', 1.00f },
    { "W", 'W', 1.00f },
    { "E", 'E', 1.00f },
    { "R", 'R', 1.00f },
};

constexpr KeyBinding kMiddleRowKeys[] =
{
    { "Shift", VK_SHIFT, 1.35f },
    { "A", 'A', 1.00f },
    { "S", 'S', 1.00f },
    { "D", 'D', 1.00f },
    { "F", 'F', 1.00f },
};

constexpr KeyBinding kBottomRowKeys[] =
{
    { "Ctrl", VK_CONTROL, 1.25f },
    { "Space", VK_SPACE, 3.20f },
    { "Alt", VK_MENU, 1.20f },
};

constexpr KeyBinding kMouseRowKeys[] =
{
    { "LMB", VK_LBUTTON, 1.25f },
    { "RMB", VK_RBUTTON, 1.25f },
    { "MMB", VK_MBUTTON, 1.25f },
};

constexpr KeyBinding kCoreBottomRowKeys[] =
{
    { "Ctrl", VK_CONTROL, 1.25f },
    { "Space", VK_SPACE, 4.60f },
};

constexpr KeyBinding kActionRowKeys[] =
{
    { "Z", 'Z', 1.00f },
    { "X", 'X', 1.00f },
    { "C", 'C', 1.00f },
    { "V", 'V', 1.00f },
    { "T", 'T', 1.00f },
    { "G", 'G', 1.00f },
};

constexpr KeyBinding kNumberRowKeys[] =
{
    { "1", '1', 1.00f },
    { "2", '2', 1.00f },
    { "3", '3', 1.00f },
    { "4", '4', 1.00f },
    { "5", '5', 1.00f },
};

constexpr const char* kLayoutPresetLabels[] =
{
    "Keyboard Only",
    "Keyboard + Mouse",
    "WASD Core",
};

constexpr std::size_t kTopRowKeyCount = sizeof(kTopRowKeys) / sizeof(kTopRowKeys[0]);
constexpr std::size_t kMiddleRowKeyCount = sizeof(kMiddleRowKeys) / sizeof(kMiddleRowKeys[0]);
constexpr std::size_t kBottomRowKeyCount = sizeof(kBottomRowKeys) / sizeof(kBottomRowKeys[0]);
constexpr std::size_t kMouseRowKeyCount = sizeof(kMouseRowKeys) / sizeof(kMouseRowKeys[0]);
constexpr std::size_t kCoreBottomRowKeyCount = sizeof(kCoreBottomRowKeys) / sizeof(kCoreBottomRowKeys[0]);
constexpr std::size_t kActionRowKeyCount = sizeof(kActionRowKeys) / sizeof(kActionRowKeys[0]);
constexpr std::size_t kNumberRowKeyCount = sizeof(kNumberRowKeys) / sizeof(kNumberRowKeys[0]);

constexpr std::array<KeyRow, 5> kExtendedRows =
{
    KeyRow{ kTopRowKeys, kTopRowKeyCount },
    KeyRow{ kMiddleRowKeys, kMiddleRowKeyCount },
    KeyRow{ kBottomRowKeys, kBottomRowKeyCount },
    KeyRow{ kActionRowKeys, kActionRowKeyCount },
    KeyRow{ kNumberRowKeys, kNumberRowKeyCount },
};

constexpr std::array<KeyRow, 6> kExtendedMouseRows =
{
    KeyRow{ kTopRowKeys, kTopRowKeyCount },
    KeyRow{ kMiddleRowKeys, kMiddleRowKeyCount },
    KeyRow{ kBottomRowKeys, kBottomRowKeyCount },
    KeyRow{ kActionRowKeys, kActionRowKeyCount },
    KeyRow{ kNumberRowKeys, kNumberRowKeyCount },
    KeyRow{ kMouseRowKeys, kMouseRowKeyCount },
};

constexpr std::array<KeyRow, 3> kWASDCoreRows =
{
    KeyRow{ kTopRowKeys, kTopRowKeyCount },
    KeyRow{ kMiddleRowKeys, kMiddleRowKeyCount },
    KeyRow{ kCoreBottomRowKeys, kCoreBottomRowKeyCount },
};
}

int GetLayoutPresetCount()
{
    return static_cast<int>(sizeof(kLayoutPresetLabels) / sizeof(kLayoutPresetLabels[0]));
}

const char* const* GetLayoutPresetLabels()
{
    return kLayoutPresetLabels;
}

KeyRowSet GetRowsForPreset(int presetIndex)
{
    // 1 号预设显示键盘加鼠标，其余预设保持既有布局。
    if (presetIndex == 1)
    {
        return KeyRowSet{ kExtendedMouseRows.data(), kExtendedMouseRows.size() };
    }

    if (presetIndex == 2)
    {
        return KeyRowSet{ kWASDCoreRows.data(), kWASDCoreRows.size() };
    }

    return KeyRowSet{ kExtendedRows.data(), kExtendedRows.size() };
}

void ApplyLayoutPresetDefaults(int presetIndex, bool& showDebugPanel, float& layoutScale)
{
    // 仅同步当前已实现的参数，避免预设切换没有反馈。
    switch (presetIndex)
    {
    case 0:
        showDebugPanel = false;
        layoutScale = 1.0f;
        break;
    case 1:
        showDebugPanel = true;
        layoutScale = 1.0f;
        break;
    case 2:
        showDebugPanel = false;
        layoutScale = 1.0f;
        break;
    default:
        break;
    }
}
} // namespace keyviz
