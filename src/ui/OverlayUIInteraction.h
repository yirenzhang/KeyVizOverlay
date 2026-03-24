#pragma once

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
    float& overlayOpacity);
} // namespace keyviz
