#pragma once

#include <cstddef>

#include "KeyboardLayoutPresets.h"

namespace keyviz
{
struct LayoutMetrics
{
    float scale = 1.0f;
    float keyWidth = 54.0f;
    float keyHeight = 34.0f;
    float keySpacing = 8.0f;
    float rowSpacing = 16.0f;
    float rowPaddingX = 14.0f;
    float rowPaddingY = 12.0f;
    float dragBarHeight = 18.0f;
};

float ClampLayoutScale(float scale);
LayoutMetrics BuildLayoutMetrics(float scale);

float GetKeyWidth(const KeyBinding& binding, const LayoutMetrics& metrics);
float GetKeyVisualWidth(const KeyBinding& binding, const LayoutMetrics& metrics);
float GetKeyVisualHeight(const LayoutMetrics& metrics);

float MeasureRowWidth(const KeyBinding* keys, std::size_t keyCount, const LayoutMetrics& metrics);
float MeasureMaxRowWidth(const LayoutMetrics& metrics, KeyRowSet rowSet);
float MeasureClusterContentHeight(const LayoutMetrics& metrics, KeyRowSet rowSet);
float MeasureClusterBackgroundHeight(const LayoutMetrics& metrics, KeyRowSet rowSet);
} // namespace keyviz
