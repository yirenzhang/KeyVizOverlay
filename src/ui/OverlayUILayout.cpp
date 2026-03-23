#include "OverlayUILayout.h"

#include <algorithm>
#include <windows.h>

#ifdef max
#undef max
#endif

#ifdef min
#undef min
#endif

namespace keyviz
{
namespace
{
constexpr float kBaseKeyWidth = 48.0f;
constexpr float kBaseKeyHeight = 34.0f;
constexpr float kBaseKeySpacing = 6.0f;
constexpr float kBaseRowSpacing = 12.0f;
constexpr float kBaseRowPaddingX = 14.0f;
constexpr float kBaseRowPaddingY = 12.0f;
constexpr float kBaseDragBarHeight = 18.0f;
constexpr float kMinLayoutScale = 0.7f;
constexpr float kMaxLayoutScale = 1.8f;

bool IsMouseVisualRow(const KeyRow& row)
{
    return row.keyCount == 3 &&
        row.keys[0].keyCode == VK_LBUTTON &&
        row.keys[1].keyCode == VK_RBUTTON &&
        row.keys[2].keyCode == VK_MBUTTON;
}

float ComputeKeyboardRowsHeight(std::size_t rowCount, const LayoutMetrics& metrics)
{
    if (rowCount == 0U)
    {
        return 0.0f;
    }

    const float count = static_cast<float>(rowCount);
    return GetKeyVisualHeight(metrics) * count + metrics.rowSpacing * (count - 1.0f);
}

float GetMouseVisualWidth(const LayoutMetrics& metrics)
{
    return metrics.keyWidth * 1.95f;
}

float GetMouseVisualHeight(const LayoutMetrics& metrics, std::size_t keyboardRowCount)
{
    const float keyboardHeight = ComputeKeyboardRowsHeight(keyboardRowCount, metrics);
    const float minimumMouseHeight = GetKeyVisualHeight(metrics) * 2.35f;
    return (std::max)(keyboardHeight, minimumMouseHeight);
}
}

float ClampLayoutScale(float scale)
{
    return std::clamp(scale, kMinLayoutScale, kMaxLayoutScale);
}

LayoutMetrics BuildLayoutMetrics(float scale)
{
    scale = ClampLayoutScale(scale);

    LayoutMetrics metrics{};
    metrics.scale = scale;
    metrics.keyWidth = kBaseKeyWidth * scale;
    metrics.keyHeight = kBaseKeyHeight * scale;
    metrics.keySpacing = kBaseKeySpacing * scale;
    metrics.rowSpacing = kBaseRowSpacing * scale;
    metrics.rowPaddingX = kBaseRowPaddingX * scale;
    metrics.rowPaddingY = kBaseRowPaddingY * scale;
    metrics.dragBarHeight = kBaseDragBarHeight * scale;
    return metrics;
}

float GetKeyWidth(const KeyBinding& binding, const LayoutMetrics& metrics)
{
    return metrics.keyWidth * binding.widthScale;
}

float GetKeyVisualWidth(const KeyBinding& binding, const LayoutMetrics& metrics)
{
    return GetKeyWidth(binding, metrics) + 2.0f * metrics.scale;
}

float GetKeyVisualHeight(const LayoutMetrics& metrics)
{
    return metrics.keyHeight + 4.0f * metrics.scale;
}

float MeasureRowWidth(const KeyBinding* keys, std::size_t keyCount, const LayoutMetrics& metrics)
{
    float rowWidth = 0.0f;
    for (std::size_t i = 0; i < keyCount; ++i)
    {
        rowWidth += GetKeyVisualWidth(keys[i], metrics);
        if (i + 1U < keyCount)
        {
            rowWidth += metrics.keySpacing;
        }
    }

    return rowWidth;
}

float MeasureMaxRowWidth(const LayoutMetrics& metrics, KeyRowSet rowSet)
{
    float keyboardMaxWidth = 0.0f;
    std::size_t keyboardRowCount = 0U;
    bool hasMouseRow = false;
    for (std::size_t rowIndex = 0; rowIndex < rowSet.count; ++rowIndex)
    {
        const KeyRow& row = rowSet.rows[rowIndex];
        if (IsMouseVisualRow(row))
        {
            hasMouseRow = true;
            continue;
        }

        keyboardMaxWidth = (std::max)(keyboardMaxWidth, MeasureRowWidth(row.keys, row.keyCount, metrics));
        ++keyboardRowCount;
    }

    if (hasMouseRow)
    {
        if (keyboardRowCount == 0U)
        {
            return GetMouseVisualWidth(metrics);
        }

        return keyboardMaxWidth + metrics.keySpacing * 1.5f + GetMouseVisualWidth(metrics);
    }

    return keyboardMaxWidth;
}

float MeasureClusterContentHeight(const LayoutMetrics& metrics, KeyRowSet rowSet)
{
    std::size_t keyboardRowCount = 0U;
    bool hasMouseRow = false;
    for (std::size_t rowIndex = 0; rowIndex < rowSet.count; ++rowIndex)
    {
        if (IsMouseVisualRow(rowSet.rows[rowIndex]))
        {
            hasMouseRow = true;
            continue;
        }
        ++keyboardRowCount;
    }

    const float keyboardHeight = ComputeKeyboardRowsHeight(keyboardRowCount, metrics);
    if (!hasMouseRow)
    {
        return keyboardHeight;
    }

    return (std::max)(keyboardHeight, GetMouseVisualHeight(metrics, keyboardRowCount));
}

float MeasureClusterBackgroundHeight(const LayoutMetrics& metrics, KeyRowSet rowSet)
{
    return MeasureClusterContentHeight(metrics, rowSet) + (metrics.rowPaddingY * 2.0f);
}
} // namespace keyviz
