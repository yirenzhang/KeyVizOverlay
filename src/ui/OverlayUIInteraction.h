#pragma once

#include <cstddef>
#include <string>

#include "OverlayPanelRenderer.h"
#include "OverlayUICallbacks.h"

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
    std::string& customStatusMessage);
} // namespace keyviz
