#include "OverlayUI.h"
#include "OverlayContentRenderer.h"
#include "OverlayRenderContext.h"
#include "OverlayStateUpdater.h"
#include "OverlayUIConfig.h"
#include "OverlayUIInteraction.h"
#include "OverlayWindowScope.h"
#include "KeyboardLayoutPresets.h"

namespace keyviz
{

void OverlayUI::Initialize()
{
    m_keyGlowEffects.clear();
    m_dragInteractionActive = false;
    InvalidateRenderContext();
}

void OverlayUI::Shutdown()
{
    m_keyGlowEffects.clear();
}

void OverlayUI::Update(float deltaSeconds, const InputService& inputService)
{
    const KeyRowSet rowSet = GetRowsForPreset(m_layoutPresetIndex);
    UpdateKeyGlowStates(deltaSeconds, inputService, rowSet, m_keyGlowEffects);
}

ImVec2 OverlayUI::GetPreferredWindowSize() const
{
    return GetRenderContext().preferredSize;
}

float OverlayUI::GetOverlayOpacity() const
{
    return m_overlayOpacity;
}

void OverlayUI::Render(const InputService& inputService)
{
    const OverlayUIConfig& uiConfig = GetOverlayUIConfig();
    const OverlayRenderContext& context = GetRenderContext();
    OverlayWindowConfig windowConfig{};
    windowConfig.title = uiConfig.overlayTitle;
    windowConfig.preferredSize = context.preferredSize;
    windowConfig.overlayOpacity = m_overlayOpacity;
    windowConfig.windowFlags = GetOverlayWindowFlags();
    OverlayWindowScope windowScope(windowConfig);
    if (!windowScope.IsContentVisible())
    {
        return;
    }

    const OverlayPanelRenderResult panelResult = RenderOverlayContent(
        inputService,
        uiConfig,
        context.panelConfig,
        context.metrics,
        context.rowSet,
        m_keyGlowEffects,
        m_overlayOpacity,
        m_layoutPresetIndex,
        m_dragInteractionActive,
        m_showDebugPanel,
        GetLayoutPresetLabels(),
        GetLayoutPresetCount());
    ApplyOverlayPanelResult(
        panelResult,
        m_interactionHandlers,
        m_dragInteractionActive,
        m_layoutPresetIndex,
        m_showDebugPanel,
        m_layoutScale,
        m_overlayOpacity);
    if (panelResult.layoutPresetChanged)
    {
        InvalidateRenderContext();
    }
}

void OverlayUI::DrawKeyboardVisualizer(const InputService& inputService)
{
    const OverlayUIConfig& uiConfig = GetOverlayUIConfig();
    const OverlayRenderContext& context = GetRenderContext();
    DrawOverlayKeyboardVisualizer(inputService, uiConfig, context.metrics, context.rowSet, m_keyGlowEffects);
}

void OverlayUI::SetShowDebugPanel(bool show)
{
    if (m_showDebugPanel == show)
    {
        return;
    }
    m_showDebugPanel = show;
    InvalidateRenderContext();
}

void OverlayUI::SetExitRequestHandler(ExitRequestHandler handler, void* context)
{
    m_interactionHandlers.exitRequestHandler = handler;
    m_interactionHandlers.exitRequestContext = context;
}

void OverlayUI::SetDragRequestHandler(DragRequestHandler handler, void* context)
{
    m_interactionHandlers.dragRequestHandler = handler;
    m_interactionHandlers.dragRequestContext = context;
}

void OverlayUI::SetDragStateRequestHandler(DragStateRequestHandler handler, void* context)
{
    m_interactionHandlers.dragStateRequestHandler = handler;
    m_interactionHandlers.dragStateRequestContext = context;
}

void OverlayUI::SetLayoutScale(float scale)
{
    const float clampedScale = ClampLayoutScale(scale);
    if (m_layoutScale == clampedScale)
    {
        return;
    }
    m_layoutScale = clampedScale;
    InvalidateRenderContext();
}

const OverlayRenderContext& OverlayUI::GetRenderContext() const
{
    if (m_renderContextDirty)
    {
        m_renderContextCache = BuildOverlayRenderContext(
            m_layoutScale,
            m_showDebugPanel,
            m_layoutPresetIndex,
            GetOverlayUIConfig());
        m_renderContextDirty = false;
    }
    return m_renderContextCache;
}

void OverlayUI::InvalidateRenderContext()
{
    m_renderContextDirty = true;
}
} // namespace keyviz
