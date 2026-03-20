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
    int layoutPresetCount)
{
    const OverlayPanelRenderResult panelResult = RenderOverlayPanelControls(
        panelConfig,
        metrics,
        overlayOpacity,
        layoutPresetIndex,
        layoutPresetLabels,
        layoutPresetCount,
        dragInteractionActive);

    ImGui::Spacing();
    ImGui::Separator();
    DrawOverlayKeyboardVisualizer(inputService, uiConfig, metrics, rowSet, keyGlowEffects);
    if (showDebugPanel)
    {
        ImGui::Separator();
        ImGui::TextUnformatted(uiConfig.debugHintText);
        ImGui::TextDisabled(uiConfig.debugTodoText);
    }

    return panelResult;
}
} // namespace keyviz
