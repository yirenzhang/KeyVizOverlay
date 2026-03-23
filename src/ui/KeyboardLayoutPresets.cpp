#include "KeyboardLayoutPresets.h"

#include <array>

#include <windows.h>

namespace keyviz
{
namespace
{
constexpr KeyBinding kTopRowKeys[] =
{
    { "TAB", VK_TAB, 0.96f },
    { "Q", 'Q', 0.86f },
    { "W", 'W', 0.86f },
    { "E", 'E', 0.86f },
    { "R", 'R', 0.86f },
};

constexpr KeyBinding kMiddleRowKeys[] =
{
    { "SHIFT", VK_SHIFT, 1.18f },
    { "A", 'A', 0.86f },
    { "S", 'S', 0.86f },
    { "D", 'D', 0.86f },
    { "F", 'F', 0.86f },
};

constexpr KeyBinding kBottomRowKeys[] =
{
    { "CTRL", VK_CONTROL, 1.02f },
    { "", VK_SPACE, 3.14f },
};

constexpr KeyBinding kMouseRowKeys[] =
{
    { "", VK_LBUTTON, 1.00f },
    { "", VK_RBUTTON, 1.00f },
    { "", VK_MBUTTON, 1.00f },
};

constexpr const char* kLayoutPresetLabels[] =
{
    "Keyboard Only",
    "Keyboard + Mouse",
};

constexpr std::size_t kTopRowKeyCount = sizeof(kTopRowKeys) / sizeof(kTopRowKeys[0]);
constexpr std::size_t kMiddleRowKeyCount = sizeof(kMiddleRowKeys) / sizeof(kMiddleRowKeys[0]);
constexpr std::size_t kBottomRowKeyCount = sizeof(kBottomRowKeys) / sizeof(kBottomRowKeys[0]);
constexpr std::size_t kMouseRowKeyCount = sizeof(kMouseRowKeys) / sizeof(kMouseRowKeys[0]);

constexpr std::array<KeyRow, 3> kKeyboardRows =
{
    KeyRow{ kTopRowKeys, kTopRowKeyCount },
    KeyRow{ kMiddleRowKeys, kMiddleRowKeyCount },
    KeyRow{ kBottomRowKeys, kBottomRowKeyCount },
};

constexpr std::array<KeyRow, 4> kKeyboardMouseRows =
{
    KeyRow{ kTopRowKeys, kTopRowKeyCount },
    KeyRow{ kMiddleRowKeys, kMiddleRowKeyCount },
    KeyRow{ kBottomRowKeys, kBottomRowKeyCount },
    KeyRow{ kMouseRowKeys, kMouseRowKeyCount },
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
    // 1 号预设显示键盘加鼠标，其余预设使用键盘布局。
    if (presetIndex == 1)
    {
        return KeyRowSet{ kKeyboardMouseRows.data(), kKeyboardMouseRows.size() };
    }

    return KeyRowSet{ kKeyboardRows.data(), kKeyboardRows.size() };
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
    default:
        break;
    }
}
} // namespace keyviz
