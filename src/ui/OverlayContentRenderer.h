#pragma once

#include <cstdint>
#include <unordered_map>

#include "KeyboardLayoutPresets.h"
#include "OverlayPanelMetrics.h"
#include "OverlayPanelRenderer.h"
#include "OverlayUIConfig.h"
#include "OverlayUILayout.h"
#include "effects/GlowEffect.h"
#include "input/InputService.h"

namespace keyviz
{
OverlayPanelRenderResult RenderOverlayContent(
    const InputService& inputService,
    const OverlayUIConfig& uiConfig,
    const OverlayPanelMetricsConfig& panelConfig,
    const LayoutMetrics& metrics,
    KeyRowSet rowSet,
    const std::unordered_map<std::uint32_t, GlowEffect>& keyGlowEffects,
    float overlayOpacity,
    int layoutPresetIndex,
    bool dragInteractionActive,
    bool showDebugPanel,
    const char* const* layoutPresetLabels,
    int layoutPresetCount);

void DrawOverlayKeyboardVisualizer(
    const InputService& inputService,
    const OverlayUIConfig& uiConfig,
    const LayoutMetrics& metrics,
    KeyRowSet rowSet,
    const std::unordered_map<std::uint32_t, GlowEffect>& keyGlowEffects);
} // namespace keyviz
