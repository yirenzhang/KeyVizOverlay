#include "OverlayContentRenderer.h"

#include "OverlayKeyRenderer.h"

namespace keyviz
{
void DrawOverlayKeyboardVisualizer(
    const InputService& inputService,
    const OverlayUIConfig& uiConfig,
    const LayoutMetrics& metrics,
    KeyRowSet rowSet,
    const std::unordered_map<std::uint32_t, GlowEffect>& keyGlowEffects)
{
    DrawKeyboardCluster(
        inputService,
        rowSet,
        metrics,
        keyGlowEffects,
        uiConfig.keyStatesSectionInset,
        uiConfig.sectionGap,
        uiConfig.keyStatesLabel,
        uiConfig.futurePlanText);
}

OverlayPanelRenderResult RenderOverlayConsole(
    const OverlayPanelMetricsConfig& panelConfig,
    const LayoutMetrics& metrics,
    float overlayOpacity,
    int layoutPresetIndex,
    bool dragInteractionActive,
    const char* const* layoutPresetLabels,
    int layoutPresetCount)
{
    return RenderOverlayPanelControls(
        panelConfig,
        metrics,
        overlayOpacity,
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
    const std::unordered_map<std::uint32_t, GlowEffect>& keyGlowEffects,
    bool showDebugPanel)
{
    ImGui::Spacing();
    DrawOverlayKeyboardVisualizer(inputService, uiConfig, metrics, rowSet, keyGlowEffects);
    if (showDebugPanel)
    {
        ImGui::Separator();
        ImGui::TextUnformatted(uiConfig.debugHintText);
        ImGui::TextDisabled(uiConfig.debugTodoText);
    }
}
} // namespace keyviz
