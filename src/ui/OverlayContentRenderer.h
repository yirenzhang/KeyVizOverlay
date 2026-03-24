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
OverlayPanelRenderResult RenderOverlayConsole(
    const OverlayPanelMetricsConfig& panelConfig,
    const LayoutMetrics& metrics,
    float overlayOpacity,
    float layoutScale,
    int layoutPresetIndex,
    bool dragInteractionActive,
    const char* const* layoutPresetLabels,
    int layoutPresetCount);

void RenderOverlayKeyStates(
    const InputService& inputService,
    const OverlayUIConfig& uiConfig,
    const LayoutMetrics& metrics,
    KeyRowSet rowSet,
    const std::unordered_map<std::uint32_t, GlowEffect>& keyGlowEffects);
} // namespace keyviz
