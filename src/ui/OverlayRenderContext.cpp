#include "OverlayRenderContext.h"

#include "KeyboardLayoutPresets.h"

namespace keyviz
{
OverlayRenderContext BuildOverlayRenderContext(
    float layoutScale,
    bool consoleHidden,
    int layoutPresetIndex,
    const OverlayUIConfig& uiConfig)
{
    OverlayRenderContext context{};
    context.metrics = BuildLayoutMetrics(layoutScale);
    context.rowSet = GetRowsForPreset(layoutPresetIndex);
    context.panelConfig = BuildOverlayPanelMetricsConfig(uiConfig);
    context.consoleWindowPos = ImVec2(0.0f, 0.0f);
    if (ImGui::GetCurrentContext() == nullptr)
    {
        const float customEditorExtraHeight = IsCustomLayoutPreset(layoutPresetIndex) ? 180.0f * context.metrics.scale : 0.0f;
        const float consoleHeight = consoleHidden ? 0.0f : (120.0f * context.metrics.scale + customEditorExtraHeight);
        const float windowGap = consoleHidden ? 0.0f : 8.0f * context.metrics.scale;
        context.windowSizes.consoleSize = ImVec2(uiConfig.minimumWindowWidth, consoleHeight);
        context.windowSizes.keyStatesSize = ImVec2(uiConfig.minimumWindowWidth, 220.0f * context.metrics.scale);
        context.windowSizes.windowGap = windowGap;
        context.windowSizes.totalSize = ImVec2(
            uiConfig.minimumWindowWidth,
            context.windowSizes.consoleSize.y + context.windowSizes.windowGap + context.windowSizes.keyStatesSize.y);
        context.keyStatesWindowPos = ImVec2(0.0f, context.windowSizes.consoleSize.y + context.windowSizes.windowGap);
        context.preferredSize = context.windowSizes.totalSize;
        return context;
    }
    context.windowSizes = ComputeOverlayWindowSizes(
        context.metrics,
        context.rowSet,
        context.panelConfig,
        IsCustomLayoutPreset(layoutPresetIndex));
    if (consoleHidden)
    {
        context.windowSizes.consoleSize.y = 0.0f;
        context.windowSizes.windowGap = 0.0f;
        context.windowSizes.totalSize = context.windowSizes.keyStatesSize;
    }
    context.keyStatesWindowPos = ImVec2(0.0f, context.windowSizes.consoleSize.y + context.windowSizes.windowGap);
    context.preferredSize = context.windowSizes.totalSize;
    return context;
}
} // namespace keyviz
