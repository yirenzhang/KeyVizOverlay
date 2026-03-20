#pragma once

#include "OverlayPanelMetrics.h"
#include "imgui.h"

namespace keyviz
{
struct OverlayUIConfig
{
    const char* overlayTitle = nullptr;
    const char* dragButtonLabel = nullptr;
    const char* exitButtonLabel = nullptr;
    const char* layoutPresetLabel = nullptr;
    const char* opacityLabel = nullptr;
    const char* debugHintText = nullptr;
    const char* debugTodoText = nullptr;
    const char* keyStatesLabel = nullptr;
    const char* futurePlanText = nullptr;

    float sectionGap = 0.0f;
    float keyStatesSectionInset = 0.0f;
    float layoutComboWidth = 0.0f;
    float opacitySliderWidth = 0.0f;
    float minimumWindowWidth = 0.0f;
    float minimumWindowHeight = 0.0f;
};

const OverlayUIConfig& GetOverlayUIConfig();
OverlayPanelMetricsConfig BuildOverlayPanelMetricsConfig(const OverlayUIConfig& config);
ImGuiWindowFlags GetOverlayWindowFlags();
} // namespace keyviz
