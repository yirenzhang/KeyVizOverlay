#include "OverlayUIInteraction.h"

#include <algorithm>
#include <cstdio>

#include "KeyboardLayoutPresets.h"
#include "OverlayUILayout.h"

namespace keyviz
{
void ApplyOverlayPanelResult(
    const OverlayPanelRenderResult& panelResult,
    OverlayUIInteractionHandlers handlers,
    bool& dragInteractionActive,
    int& layoutPresetIndex,
    float& layoutScale,
    float& overlayOpacity,
    bool& customEditMode,
    int& customPaletteIndex,
    int& customTargetRowIndex,
    bool& customIncludeMouse,
    int& customPresetFileIndex,
    char* customPresetNameBuffer,
    std::size_t customPresetNameBufferSize,
    std::string& customStatusMessage)
{
    auto setStatus = [&](const char* text)
    {
        customStatusMessage = text != nullptr ? text : "";
    };
    bool syncNameWithSelectedPreset = false;

    dragInteractionActive = panelResult.dragInteractionActive;
    if (panelResult.dragStateChanged && handlers.dragStateRequestHandler != nullptr)
    {
        handlers.dragStateRequestHandler(handlers.dragStateRequestContext, dragInteractionActive);
    }
    if (panelResult.hasDragDelta && handlers.dragRequestHandler != nullptr)
    {
        handlers.dragRequestHandler(handlers.dragRequestContext, panelResult.dragDeltaX, panelResult.dragDeltaY);
    }
    if (panelResult.layoutPresetChanged)
    {
        layoutPresetIndex = panelResult.layoutPresetIndex;
        ApplyLayoutPresetDefaults(layoutPresetIndex, layoutScale);
    }
    if (panelResult.layoutScaleChanged)
    {
        layoutScale = ClampLayoutScale(panelResult.layoutScale);
    }
    if (panelResult.overlayOpacityChanged)
    {
        overlayOpacity = panelResult.overlayOpacity;
    }
    if (panelResult.customEditModeChanged)
    {
        customEditMode = panelResult.customEditMode;
    }
    customPaletteIndex = panelResult.customPaletteIndex;
    customTargetRowIndex = panelResult.customTargetRowIndex;
    const int previousPresetFileIndex = customPresetFileIndex;
    customPresetFileIndex = panelResult.customPresetFileIndex;
    if (customPresetFileIndex != previousPresetFileIndex)
    {
        syncNameWithSelectedPreset = true;
    }
    if (panelResult.customIncludeMouseChanged)
    {
        customIncludeMouse = panelResult.customIncludeMouse;
        SetCustomIncludeMouse(customIncludeMouse);
        setStatus(customIncludeMouse ? "Mouse visual enabled" : "Mouse visual disabled");
    }
    if (panelResult.customAddRequested)
    {
        setStatus(AddCustomKeyByPaletteIndex(customPaletteIndex, customTargetRowIndex) ? "Key added" : "Failed to add key");
    }
    if (panelResult.customAddRowRequested)
    {
        setStatus(AddCustomRow() ? "Row added" : "Cannot add more rows");
    }
    if (panelResult.customRemoveRowRequested)
    {
        setStatus(RemoveCustomRow(panelResult.customRemoveRowIndex) ? "Row deleted" : "Cannot delete row");
    }
    if (panelResult.customLoadPresetRequested)
    {
        const bool ok = LoadCustomPresetFileByIndex(customPresetFileIndex);
        setStatus(ok ? "Preset file loaded" : "Failed to load preset file");
        syncNameWithSelectedPreset = syncNameWithSelectedPreset || ok;
        if (ok)
        {
            customIncludeMouse = GetCustomIncludeMouse();
        }
    }
    if (panelResult.customSaveAsRequested)
    {
        const bool ok = SaveCustomPresetAs(customPresetNameBuffer);
        setStatus(ok ? "Preset file saved" : "Save As failed");
        syncNameWithSelectedPreset = syncNameWithSelectedPreset || ok;
    }
    if (panelResult.customDuplicateRequested)
    {
        const bool ok = DuplicateCustomPresetFile(customPresetFileIndex, customPresetNameBuffer);
        setStatus(ok ? "Preset file duplicated" : "Duplicate failed");
        syncNameWithSelectedPreset = syncNameWithSelectedPreset || ok;
    }
    if (panelResult.customRenameRequested)
    {
        const bool ok = RenameCustomPresetFile(customPresetFileIndex, customPresetNameBuffer);
        setStatus(ok ? "Preset file renamed" : "Rename failed");
        syncNameWithSelectedPreset = syncNameWithSelectedPreset || ok;
    }
    if (panelResult.customDeleteRequested)
    {
        const bool ok = DeleteCustomPresetFile(customPresetFileIndex);
        setStatus(ok ? "Preset file deleted" : "Delete failed");
        syncNameWithSelectedPreset = syncNameWithSelectedPreset || ok;
    }
    if (panelResult.customResetRequested)
    {
        ResetCustomLayoutPreset();
        setStatus("Custom preset reset");
    }

    if (syncNameWithSelectedPreset && customPresetNameBuffer != nullptr && customPresetNameBufferSize > 0U)
    {
        const int presetCount = GetCustomPresetFileCount();
        const char* const* presetLabels = GetCustomPresetFileLabels();
        if (presetCount > 0 && presetLabels != nullptr)
        {
            customPresetFileIndex = (std::clamp)(customPresetFileIndex, 0, presetCount - 1);
            std::snprintf(
                customPresetNameBuffer,
                customPresetNameBufferSize,
                "%s",
                presetLabels[customPresetFileIndex]);
        }
    }
    if (panelResult.requestExit && handlers.exitRequestHandler != nullptr)
    {
        handlers.exitRequestHandler(handlers.exitRequestContext);
    }
}
} // namespace keyviz
