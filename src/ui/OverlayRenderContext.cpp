#include "OverlayRenderContext.h"

#include "KeyboardLayoutPresets.h"

namespace keyviz
{
OverlayRenderContext BuildOverlayRenderContext(
    float layoutScale,
    bool showDebugPanel,
    int layoutPresetIndex,
    const OverlayUIConfig& uiConfig)
{
    OverlayRenderContext context{};
    context.metrics = BuildLayoutMetrics(layoutScale);
    context.rowSet = GetRowsForPreset(layoutPresetIndex);
    context.panelConfig = BuildOverlayPanelMetricsConfig(uiConfig);
    if (ImGui::GetCurrentContext() == nullptr)
    {
        context.preferredSize = ImVec2(uiConfig.minimumWindowWidth, uiConfig.minimumWindowHeight);
        return context;
    }
    context.preferredSize = ComputeOverlayWindowSize(
        context.metrics,
        showDebugPanel,
        context.rowSet,
        context.panelConfig);
    return context;
}
} // namespace keyviz
