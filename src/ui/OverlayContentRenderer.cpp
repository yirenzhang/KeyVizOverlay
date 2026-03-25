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
    int layoutPresetCount,
    const OverlayPanelCustomLayoutState& customLayoutState)
{
    return RenderOverlayPanelControls(
        panelConfig,
        metrics,
        overlayOpacity,
        layoutScale,
        layoutPresetIndex,
        layoutPresetLabels,
        layoutPresetCount,
        dragInteractionActive,
        customLayoutState);
}

void RenderOverlayKeyStates(
    const InputService& inputService,
    const OverlayUIConfig& uiConfig,
    const LayoutMetrics& metrics,
    KeyRowSet rowSet,
    const std::unordered_map<std::uint32_t, GlowEffect>& keyGlowEffects,
    OverlayKeyLayoutEditState* editState)
{
    ImGui::Spacing();
    DrawKeyboardCluster(
        inputService,
        rowSet,
        metrics,
        keyGlowEffects,
        uiConfig.keyStatesSectionInset,
        uiConfig.sectionGap,
        uiConfig.keyStatesLabel,
        editState);
}
} // namespace keyviz
