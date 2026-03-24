#pragma once

#include "KeyboardLayoutPresets.h"
#include "OverlayUILayout.h"
#include "imgui.h"

namespace keyviz
{
struct OverlayPanelMetricsConfig
{
    const char* title = nullptr;
    const char* dragButtonLabel = nullptr;
    const char* exitButtonLabel = nullptr;
    const char* layoutLabel = nullptr;
    const char* opacityLabel = nullptr;
    const char* keySizeLabel = nullptr;

    float keyStatesSectionInset = 16.0f;
    float layoutComboWidth = 164.0f;
    float opacitySliderWidth = 150.0f;
    float keySizeSliderWidth = 150.0f;
    float minimumWindowWidth = 420.0f;
    float minimumWindowHeight = 240.0f;
    float sectionGap = 16.0f;
};

struct OverlayWindowSizes
{
    ImVec2 consoleSize{};
    ImVec2 keyStatesSize{};
    ImVec2 totalSize{};
    float windowGap = 0.0f;
};

float MeasurePanelButtonWidth(const char* label, const LayoutMetrics& metrics);
OverlayWindowSizes ComputeOverlayWindowSizes(
    const LayoutMetrics& metrics,
    KeyRowSet rowSet,
    const OverlayPanelMetricsConfig& config);
} // namespace keyviz
