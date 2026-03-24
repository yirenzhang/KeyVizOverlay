#pragma once

#include "OverlayPanelMetrics.h"
#include "OverlayUIConfig.h"
#include "OverlayUILayout.h"

namespace keyviz
{
struct OverlayRenderContext
{
    LayoutMetrics metrics{};
    KeyRowSet rowSet{};
    OverlayPanelMetricsConfig panelConfig{};
    OverlayWindowSizes windowSizes{};
    ImVec2 consoleWindowPos{};
    ImVec2 keyStatesWindowPos{};
    ImVec2 preferredSize{};
};

OverlayRenderContext BuildOverlayRenderContext(
    float layoutScale,
    bool consoleHidden,
    int layoutPresetIndex,
    const OverlayUIConfig& uiConfig);
} // namespace keyviz
