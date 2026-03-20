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
    ImVec2 preferredSize{};
};

OverlayRenderContext BuildOverlayRenderContext(
    float layoutScale,
    bool showDebugPanel,
    int layoutPresetIndex,
    const OverlayUIConfig& uiConfig);
} // namespace keyviz
