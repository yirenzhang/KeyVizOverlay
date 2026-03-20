#include "OverlayUILayout.h"

#include <algorithm>

namespace keyviz
{
namespace
{
constexpr float kBaseKeyWidth = 54.0f;
constexpr float kBaseKeyHeight = 34.0f;
constexpr float kBaseKeySpacing = 8.0f;
constexpr float kBaseRowSpacing = 16.0f;
constexpr float kBaseRowPaddingX = 14.0f;
constexpr float kBaseRowPaddingY = 12.0f;
constexpr float kBaseDragBarHeight = 18.0f;
constexpr float kMinLayoutScale = 0.7f;
constexpr float kMaxLayoutScale = 1.8f;
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
    return GetKeyWidth(binding, metrics) + 8.0f * metrics.scale;
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
    float maxWidth = 0.0f;
    for (std::size_t rowIndex = 0; rowIndex < rowSet.count; ++rowIndex)
    {
        const KeyRow& row = rowSet.rows[rowIndex];
        maxWidth = (std::max)(maxWidth, MeasureRowWidth(row.keys, row.keyCount, metrics));
    }
    return maxWidth;
}

float MeasureClusterContentHeight(const LayoutMetrics& metrics, KeyRowSet rowSet)
{
    const float rowCount = static_cast<float>(rowSet.count);
    return (GetKeyVisualHeight(metrics) * rowCount) + (metrics.rowSpacing * (rowCount - 1.0f));
}

float MeasureClusterBackgroundHeight(const LayoutMetrics& metrics, KeyRowSet rowSet)
{
    return MeasureClusterContentHeight(metrics, rowSet) + (metrics.rowPaddingY * 2.0f);
}
} // namespace keyviz
