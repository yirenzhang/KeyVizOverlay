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
    bool dragInteractionActive,
    const OverlayPanelCustomLayoutState& customLayoutState)
{
    OverlayPanelRenderResult result{};
    result.layoutPresetIndex = layoutPresetIndex;
    result.overlayOpacity = overlayOpacity;
    result.layoutScale = layoutScale;
    result.dragInteractionActive = dragInteractionActive;
    result.customEditMode = customLayoutState.editMode;
    result.customPaletteIndex = customLayoutState.paletteIndex;
    result.customTargetRowIndex = customLayoutState.targetRowIndex;
    result.customRemoveRowIndex = customLayoutState.targetRowIndex;
    result.customIncludeMouse = customLayoutState.includeMouse;

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
    int keyScaleMultiplier = static_cast<int>(result.layoutScale + 0.5f);
    if (ImGui::SliderInt("##KeySizeScale", &keyScaleMultiplier, 1, 4, "%dx"))
    {
        result.layoutScale = static_cast<float>(keyScaleMultiplier);
        result.layoutScaleChanged = true;
    }
    ImGui::EndGroup();

    if (customLayoutState.isCustomPreset)
    {
        ImGui::Spacing();
        ImGui::Separator();
        ImGui::Spacing();
        ImGui::TextUnformatted("Custom preset editor");

        bool editMode = result.customEditMode;
        if (ImGui::Checkbox("Enable key drag editing", &editMode))
        {
            result.customEditMode = editMode;
            result.customEditModeChanged = true;
        }

        if (customLayoutState.paletteLabels != nullptr && customLayoutState.paletteCount > 0)
        {
            ImGui::SetNextItemWidth(config.layoutComboWidth * metrics.scale);
            ImGui::Combo(
                "##CustomKeyPalette",
                &result.customPaletteIndex,
                customLayoutState.paletteLabels,
                customLayoutState.paletteCount);
        }

        if (customLayoutState.rowLabels != nullptr && customLayoutState.rowCount > 0)
        {
            ImGui::SetNextItemWidth(config.layoutComboWidth * metrics.scale);
            ImGui::Combo(
                "##CustomTargetRow",
                &result.customTargetRowIndex,
                customLayoutState.rowLabels,
                customLayoutState.rowCount);
            result.customRemoveRowIndex = result.customTargetRowIndex;
        }

        bool includeMouse = result.customIncludeMouse;
        if (ImGui::Checkbox("Include mouse visual", &includeMouse))
        {
            result.customIncludeMouse = includeMouse;
            result.customIncludeMouseChanged = true;
        }

        if (ImGui::Button("Add key to row"))
        {
            result.customAddRequested = true;
        }
        ImGui::SameLine();
        if (ImGui::Button("Add row"))
        {
            result.customAddRowRequested = true;
        }
        ImGui::SameLine();
        if (ImGui::Button("Delete row"))
        {
            result.customRemoveRowRequested = true;
        }

        if (ImGui::Button("Export custom"))
        {
            result.customExportRequested = true;
        }
        ImGui::SameLine();
        if (ImGui::Button("Import custom"))
        {
            result.customImportRequested = true;
        }
        ImGui::SameLine();
        if (ImGui::Button("Reset custom preset"))
        {
            result.customResetRequested = true;
        }
    }

    return result;
}
} // namespace keyviz
