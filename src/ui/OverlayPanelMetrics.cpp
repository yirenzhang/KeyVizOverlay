#include "OverlayPanelMetrics.h"

#include <algorithm>

namespace keyviz
{
namespace
{
float MeasureHeaderRowWidth(const LayoutMetrics& metrics, const OverlayPanelMetricsConfig& config)
{
    const ImGuiStyle& style = ImGui::GetStyle();
    const float titleWidth = ImGui::CalcTextSize(config.title).x;
    const float dragButtonWidth = MeasurePanelButtonWidth(config.dragButtonLabel, metrics);
    const float exitButtonWidth = MeasurePanelButtonWidth(config.exitButtonLabel, metrics);
    return titleWidth + style.ItemSpacing.x + dragButtonWidth + style.ItemSpacing.x + exitButtonWidth;
}

float MeasureControlsRowWidth(const LayoutMetrics& metrics, const OverlayPanelMetricsConfig& config)
{
    const ImGuiStyle& style = ImGui::GetStyle();
    const float layoutLabelWidth = ImGui::CalcTextSize(config.layoutLabel).x;
    const float opacityLabelWidth = ImGui::CalcTextSize(config.opacityLabel).x;
    const float layoutGroupWidth = (std::max)(layoutLabelWidth, config.layoutComboWidth * metrics.scale);
    const float opacityGroupWidth = (std::max)(opacityLabelWidth, config.opacitySliderWidth * metrics.scale);
    return layoutGroupWidth + style.ItemSpacing.x + opacityGroupWidth;
}

float MeasureControlsRowHeight(const LayoutMetrics&)
{
    return ImGui::GetTextLineHeight() + ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeight();
}

float MeasureContentWidth(const LayoutMetrics& metrics, bool showDebugPanel, KeyRowSet rowSet, const OverlayPanelMetricsConfig& config)
{
    const float headerWidth = MeasureHeaderRowWidth(metrics, config);
    const float controlsWidth = MeasureControlsRowWidth(metrics, config);
    const float clusterWidth = MeasureMaxRowWidth(metrics, rowSet) + metrics.rowPaddingX * 2.0f + (config.keyStatesSectionInset * metrics.scale);
    const float footerWidth = ImGui::CalcTextSize(config.footerText).x;

    float contentWidth = (std::max)(headerWidth, (std::max)(controlsWidth, (std::max)(clusterWidth, footerWidth)));
    if (showDebugPanel)
    {
        const float debugWidth = ImGui::CalcTextSize(config.debugHintText).x;
        contentWidth = (std::max)(contentWidth, debugWidth);
    }

    return contentWidth;
}
}

float MeasurePanelButtonWidth(const char* label, const LayoutMetrics& metrics)
{
    const ImGuiStyle& style = ImGui::GetStyle();
    return ImGui::CalcTextSize(label).x + style.FramePadding.x * 2.0f + 18.0f * metrics.scale;
}

ImVec2 ComputeOverlayWindowSize(
    const LayoutMetrics& metrics,
    bool showDebugPanel,
    KeyRowSet rowSet,
    const OverlayPanelMetricsConfig& config)
{
    // 根据当前预设与显示项动态估算窗口尺寸。
    const ImGuiStyle& style = ImGui::GetStyle();
    const float contentWidth = MeasureContentWidth(metrics, showDebugPanel, rowSet, config);
    const float headerRowHeight = (std::max)(metrics.dragBarHeight, ImGui::GetTextLineHeight());
    const float controlsRowHeight = MeasureControlsRowHeight(metrics);
    const float clusterHeight = MeasureClusterBackgroundHeight(metrics, rowSet);
    const float keyboardFooterHeight = ImGui::GetTextLineHeightWithSpacing();
    const float debugHeight = showDebugPanel ? (ImGui::GetTextLineHeightWithSpacing() * 2.0f) : 0.0f;

    float windowWidth = contentWidth + style.WindowPadding.x * 2.0f + 24.0f * metrics.scale;
    float windowHeight =
        style.WindowPadding.y * 2.0f +
        headerRowHeight +
        style.ItemSpacing.y +
        controlsRowHeight +
        style.ItemSpacing.y +
        style.ItemSpacing.y + 1.0f * metrics.scale +
        ImGui::GetTextLineHeight() +
        config.sectionGap * metrics.scale +
        clusterHeight +
        style.ItemSpacing.y +
        keyboardFooterHeight +
        debugHeight +
        24.0f * metrics.scale;

    windowWidth = (std::max)(windowWidth, config.minimumWindowWidth);
    windowHeight = (std::max)(windowHeight, config.minimumWindowHeight);
    return ImVec2(windowWidth, windowHeight);
}
} // namespace keyviz
