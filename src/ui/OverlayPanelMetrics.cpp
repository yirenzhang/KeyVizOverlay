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
    const float helpButtonWidth = MeasurePanelButtonWidth(config.helpButtonLabel, metrics);
    const float aboutButtonWidth = MeasurePanelButtonWidth(config.aboutButtonLabel, metrics);
    const float exitButtonWidth = MeasurePanelButtonWidth(config.exitButtonLabel, metrics);
    return titleWidth + style.ItemSpacing.x + dragButtonWidth + style.ItemSpacing.x + helpButtonWidth + style.ItemSpacing.x + aboutButtonWidth + style.ItemSpacing.x + exitButtonWidth;
}

float MeasureControlsRowWidth(const LayoutMetrics& metrics, const OverlayPanelMetricsConfig& config)
{
    const float layoutLabelWidth = ImGui::CalcTextSize(config.layoutLabel).x;
    const float opacityLabelWidth = ImGui::CalcTextSize(config.opacityLabel).x;
    const float keySizeLabelWidth = ImGui::CalcTextSize(config.keySizeLabel).x;
    const float layoutGroupWidth = (std::max)(layoutLabelWidth, config.layoutComboWidth * metrics.scale);
    const float opacityGroupWidth = (std::max)(opacityLabelWidth, config.opacitySliderWidth * metrics.scale);
    const float keySizeGroupWidth = (std::max)(keySizeLabelWidth, config.keySizeSliderWidth * metrics.scale);
    return (std::max)(layoutGroupWidth + ImGui::GetStyle().ItemSpacing.x + opacityGroupWidth, keySizeGroupWidth);
}

float MeasureControlsRowHeight(const LayoutMetrics&)
{
    const ImGuiStyle& style = ImGui::GetStyle();
    const float groupHeight = ImGui::GetTextLineHeight() + style.ItemSpacing.y + ImGui::GetFrameHeight();
    return groupHeight + style.ItemSpacing.y + groupHeight;
}

float MeasureCustomEditorHeight(const LayoutMetrics&)
{
    const ImGuiStyle& style = ImGui::GetStyle();
    const float textHeight = ImGui::GetTextLineHeight();
    const float frameHeight = ImGui::GetFrameHeight();
    // 自定义编辑区包含编辑操作 + 文件列表 + 文件名输入 + 多组按钮，预留额外间距避免裁切。
    return textHeight * 2.0f + frameHeight * 10.0f + style.ItemSpacing.y * 18.0f;
}

}

float MeasurePanelButtonWidth(const char* label, const LayoutMetrics& metrics)
{
    const ImGuiStyle& style = ImGui::GetStyle();
    return ImGui::CalcTextSize(label).x + style.FramePadding.x * 2.0f + 18.0f * metrics.scale;
}

OverlayWindowSizes ComputeOverlayWindowSizes(
    const LayoutMetrics& metrics,
    KeyRowSet rowSet,
    const OverlayPanelMetricsConfig& config,
    bool showCustomLayoutEditor)
{
    const ImGuiStyle& style = ImGui::GetStyle();
    const float headerWidth = MeasureHeaderRowWidth(metrics, config);
    const float controlsWidth = MeasureControlsRowWidth(metrics, config);
    const float clusterWidth = MeasureMaxRowWidth(metrics, rowSet) + metrics.rowPaddingX * 2.0f + (config.keyStatesSectionInset * metrics.scale);

    const float consoleContentWidth = (std::max)(headerWidth, controlsWidth);
    const float keyStatesContentWidth = clusterWidth;

    float targetWindowWidth = (std::max)(consoleContentWidth, keyStatesContentWidth);
    targetWindowWidth = targetWindowWidth + style.WindowPadding.x * 2.0f + 24.0f * metrics.scale;
    targetWindowWidth = (std::max)(targetWindowWidth, config.minimumWindowWidth);

    const float headerRowHeight = (std::max)(metrics.dragBarHeight, ImGui::GetTextLineHeight());
    const float controlsRowHeight = MeasureControlsRowHeight(metrics);
    const float customEditorHeight = showCustomLayoutEditor ? MeasureCustomEditorHeight(metrics) : 0.0f;
    const float clusterHeight = MeasureClusterBackgroundHeight(metrics, rowSet);

    const float consoleWindowHeight =
        style.WindowPadding.y * 2.0f +
        headerRowHeight +
        style.ItemSpacing.y +
        controlsRowHeight +
        customEditorHeight +
        style.ItemSpacing.y + 1.0f * metrics.scale +
        24.0f * metrics.scale;

    const float keyStatesWindowHeight =
        style.WindowPadding.y * 2.0f +
        ImGui::GetTextLineHeight() +
        config.sectionGap * metrics.scale +
        clusterHeight +
        24.0f * metrics.scale;

    const float windowGap = 8.0f * metrics.scale;
    float totalWindowHeight = consoleWindowHeight + windowGap + keyStatesWindowHeight;
    totalWindowHeight = (std::max)(totalWindowHeight, config.minimumWindowHeight);

    OverlayWindowSizes sizes{};
    sizes.consoleSize = ImVec2(targetWindowWidth, consoleWindowHeight);
    sizes.keyStatesSize = ImVec2(targetWindowWidth, keyStatesWindowHeight);
    sizes.totalSize = ImVec2(targetWindowWidth, totalWindowHeight);
    sizes.windowGap = windowGap;
    return sizes;
}

} // namespace keyviz
