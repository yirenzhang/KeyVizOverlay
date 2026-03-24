#include "OverlayContentRenderer.h"

#include "OverlayKeyRenderer.h"

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
    int layoutPresetCount)
{
    return RenderOverlayPanelControls(
        panelConfig,
        metrics,
        overlayOpacity,
        layoutScale,
        layoutPresetIndex,
        layoutPresetLabels,
        layoutPresetCount,
        dragInteractionActive);
}

void RenderOverlayKeyStates(
    const InputService& inputService,
    const OverlayUIConfig& uiConfig,
    const LayoutMetrics& metrics,
    KeyRowSet rowSet,
    const std::unordered_map<std::uint32_t, GlowEffect>& keyGlowEffects)
{
    ImGui::Spacing();
    DrawKeyboardCluster(
        inputService,
        rowSet,
        metrics,
        keyGlowEffects,
        uiConfig.keyStatesSectionInset,
        uiConfig.sectionGap,
        uiConfig.keyStatesLabel);
}
} // namespace keyviz
