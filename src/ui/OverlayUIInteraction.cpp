#include "OverlayUIInteraction.h"

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
    float& overlayOpacity)
{
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
    if (panelResult.requestExit && handlers.exitRequestHandler != nullptr)
    {
        handlers.exitRequestHandler(handlers.exitRequestContext);
    }
}
} // namespace keyviz
