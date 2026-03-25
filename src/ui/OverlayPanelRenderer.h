#pragma once

#include "OverlayPanelMetrics.h"
#include "OverlayUILayout.h"
#include "imgui.h"

namespace keyviz
{
struct OverlayPanelRenderResult
{
    bool requestExit = false;

    bool dragStateChanged = false;
    bool dragInteractionActive = false;
    bool hasDragDelta = false;
    int dragDeltaX = 0;
    int dragDeltaY = 0;

    bool layoutPresetChanged = false;
    int layoutPresetIndex = 0;

    bool overlayOpacityChanged = false;
    float overlayOpacity = 0.0f;

    bool layoutScaleChanged = false;
    float layoutScale = 1.0f;

    bool customEditModeChanged = false;
    bool customEditMode = false;

    bool customAddRequested = false;
    int customPaletteIndex = 0;
    int customTargetRowIndex = 0;

    bool customAddRowRequested = false;
    bool customRemoveRowRequested = false;
    int customRemoveRowIndex = 0;

    bool customIncludeMouseChanged = false;
    bool customIncludeMouse = true;

    int customPresetFileIndex = 0;
    bool customLoadPresetRequested = false;
    bool customSaveAsRequested = false;
    bool customDuplicateRequested = false;
    bool customRenameRequested = false;
    bool customDeleteRequested = false;

    bool customResetRequested = false;
};

struct OverlayPanelCustomLayoutState
{
    bool isCustomPreset = false;
    bool editMode = false;
    int paletteIndex = 0;
    int targetRowIndex = 0;
    const char* const* paletteLabels = nullptr;
    int paletteCount = 0;
    const char* const* rowLabels = nullptr;
    int rowCount = 0;
    bool includeMouse = true;
    const char* const* presetFileLabels = nullptr;
    int presetFileCount = 0;
    int presetFileIndex = 0;
    char* presetNameBuffer = nullptr;
    int presetNameBufferSize = 0;
    const char* statusMessage = nullptr;
};

OverlayPanelRenderResult RenderOverlayPanelControls(
    const OverlayPanelMetricsConfig& config,
    const LayoutMetrics& metrics,
    float overlayOpacity,
    float layoutScale,
    int layoutPresetIndex,
    const char* const* layoutPresetLabels,
    int layoutPresetCount,
    bool dragInteractionActive,
    const OverlayPanelCustomLayoutState& customLayoutState);
} // namespace keyviz
