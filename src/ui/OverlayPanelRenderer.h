#pragma once

#include "OverlayPanelMetrics.h"
#include "OverlayUILayout.h"
#include "imgui.h"

namespace keyviz
{
struct OverlayPanelRenderResult
{
    bool requestExit = false;

    bool dragStateChanged = false;
    bool dragInteractionActive = false;
    bool hasDragDelta = false;
    int dragDeltaX = 0;
    int dragDeltaY = 0;

    bool layoutPresetChanged = false;
    int layoutPresetIndex = 0;

    bool overlayOpacityChanged = false;
    float overlayOpacity = 0.0f;

    bool layoutScaleChanged = false;
    float layoutScale = 1.0f;
};

OverlayPanelRenderResult RenderOverlayPanelControls(
    const OverlayPanelMetricsConfig& config,
    const LayoutMetrics& metrics,
    float overlayOpacity,
    float layoutScale,
    int layoutPresetIndex,
    const char* const* layoutPresetLabels,
    int layoutPresetCount,
    bool dragInteractionActive);
} // namespace keyviz
