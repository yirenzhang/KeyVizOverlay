#include "OverlayPanelRenderer.h"

#include "OverlayPanelMetrics.h"

namespace keyviz
{
OverlayPanelRenderResult RenderOverlayPanelControls(
    const OverlayPanelMetricsConfig& config,
    const LayoutMetrics& metrics,
    float overlayOpacity,
    float layoutScale,
    int layoutPresetIndex,
    const char* const* layoutPresetLabels,
    int layoutPresetCount,
    bool dragInteractionActive)
{
    OverlayPanelRenderResult result{};
    result.layoutPresetIndex = layoutPresetIndex;
    result.overlayOpacity = overlayOpacity;
    result.layoutScale = layoutScale;
    result.dragInteractionActive = dragInteractionActive;

    ImGui::TextUnformatted(config.title);
    ImGui::SameLine();

    const float dragButtonWidth = MeasurePanelButtonWidth(config.dragButtonLabel, metrics);
    ImGui::Button(config.dragButtonLabel, ImVec2(dragButtonWidth, metrics.dragBarHeight));

    const bool dragPressed = ImGui::IsItemActive() && ImGui::IsMouseDown(ImGuiMouseButton_Left);
    if (dragPressed != result.dragInteractionActive)
    {
        result.dragInteractionActive = dragPressed;
        result.dragStateChanged = true;
    }

    if (result.dragInteractionActive && ImGui::IsMouseDragging(ImGuiMouseButton_Left))
    {
        const ImVec2 mouseDelta = ImGui::GetIO().MouseDelta;
        if (mouseDelta.x != 0.0f || mouseDelta.y != 0.0f)
        {
            result.hasDragDelta = true;
            result.dragDeltaX = static_cast<int>(mouseDelta.x);
            result.dragDeltaY = static_cast<int>(mouseDelta.y);
        }
        ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeAll);
    }

    ImGui::SameLine();
    const float exitButtonWidth = MeasurePanelButtonWidth(config.exitButtonLabel, metrics);
    const float exitButtonX = ImGui::GetCursorPosX() + ImGui::GetContentRegionAvail().x - exitButtonWidth;
    ImGui::SetCursorPosX(exitButtonX > ImGui::GetCursorPosX() ? exitButtonX : ImGui::GetCursorPosX());
    if (ImGui::Button(config.exitButtonLabel, ImVec2(exitButtonWidth, 0.0f)))
    {
        result.requestExit = true;
    }

    ImGui::Spacing();
    ImGui::Separator();

    ImGui::BeginGroup();
    ImGui::TextUnformatted(config.layoutLabel);
    ImGui::SetNextItemWidth(config.layoutComboWidth * metrics.scale);
    if (ImGui::Combo("##LayoutPreset", &result.layoutPresetIndex, layoutPresetLabels, layoutPresetCount))
    {
        result.layoutPresetChanged = true;
    }
    ImGui::EndGroup();

    ImGui::SameLine();
    ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 1.0f);
    ImGui::BeginGroup();
    ImGui::TextUnformatted(config.opacityLabel);
    ImGui::SetNextItemWidth(config.opacitySliderWidth * metrics.scale);

    int opacityPercent = static_cast<int>(result.overlayOpacity * 100.0f + 0.5f);
    if (ImGui::SliderInt("##OverlayOpacity", &opacityPercent, 0, 100, "%d%%"))
    {
        result.overlayOpacity = static_cast<float>(opacityPercent) / 100.0f;
        result.overlayOpacityChanged = true;
    }

    ImGui::EndGroup();
    ImGui::PopStyleVar();

    ImGui::Spacing();
    ImGui::BeginGroup();
    ImGui::TextUnformatted(config.keySizeLabel);
    ImGui::SetNextItemWidth(config.keySizeSliderWidth * metrics.scale);
    int keySizePercent = static_cast<int>(result.layoutScale * 100.0f + 0.5f);
    // 与布局缩放下限保持一致，避免滑条显示值与实际值反复冲突。
    if (ImGui::SliderInt("##KeySizeScale", &keySizePercent, 70, 180, "%d%%"))
    {
        result.layoutScale = static_cast<float>(keySizePercent) / 100.0f;
        result.layoutScaleChanged = true;
    }
    ImGui::EndGroup();

    return result;
}
} // namespace keyviz
