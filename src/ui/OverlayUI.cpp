#include "OverlayUI.h"

#include <algorithm>
#include <cmath>
#include <cstdio>

#include "OverlayContentRenderer.h"
#include "OverlayRenderContext.h"
#include "OverlayStateUpdater.h"
#include "OverlayUIConfig.h"
#include "OverlayUIInteraction.h"
#include "OverlayWindowScope.h"
#include "KeyboardLayoutPresets.h"

namespace keyviz
{
namespace
{
constexpr const char* kConsoleWindowTitle = "KeyViz Console";
constexpr const char* kKeyStatesWindowTitle = "Key states";
}

void OverlayUI::Initialize()
{
    m_keyGlowEffects.clear();
    // 历史配置中可能残留已移除预设索引，这里统一回落到默认预设。
    if (m_renderState.layoutPresetIndex < 0 || m_renderState.layoutPresetIndex >= GetLayoutPresetCount())
    {
        m_renderState.layoutPresetIndex = 0;
    }
    m_renderState.consoleHidden = false;
    m_consoleCommandTracker.Reset();
    m_interactionState.dragInteractionActive = false;
    m_interactionState.customEditMode = false;
    m_interactionState.customIncludeMouse = GetCustomIncludeMouse();
    m_interactionState.customPaletteIndex = 0;
    m_interactionState.customTargetRowIndex = 0;
    m_interactionState.customPresetFileIndex = 0;
    std::snprintf(m_interactionState.customPresetNameBuffer.data(), m_interactionState.customPresetNameBuffer.size(), "%s", "new_preset");
    m_interactionState.customStatusMessage.clear();
    m_interactionState.keyLayoutEditState = OverlayKeyLayoutEditState{};
    InvalidateRenderContext();
}

void OverlayUI::Shutdown()
{
    m_keyGlowEffects.clear();
    m_consoleCommandTracker.Reset();
}

void OverlayUI::Update(float deltaSeconds, const InputService& inputService)
{
    const KeyRowSet rowSet = GetRowsForPreset(m_renderState.layoutPresetIndex);
    UpdateKeyGlowStates(deltaSeconds, inputService, rowSet, m_keyGlowEffects);
    UpdateConsoleCommandState(inputService);
}

ImVec2 OverlayUI::GetPreferredWindowSize() const
{
    return GetRenderContext().preferredSize;
}

float OverlayUI::GetOverlayOpacity() const
{
    return m_renderState.overlayOpacity;
}

bool OverlayUI::IsConsoleHidden() const
{
    return m_renderState.consoleHidden;
}

void OverlayUI::Render(const InputService& inputService)
{
    const OverlayUIConfig& uiConfig = GetOverlayUIConfig();
    const OverlayRenderContext* context = &GetRenderContext();
    if (!m_renderState.consoleHidden)
    {
        OverlayWindowConfig consoleWindowConfig{};
        consoleWindowConfig.title = kConsoleWindowTitle;
        consoleWindowConfig.position = context->consoleWindowPos;
        consoleWindowConfig.preferredSize = context->windowSizes.consoleSize;
        consoleWindowConfig.overlayOpacity = 1.0f;
        consoleWindowConfig.windowFlags = GetOverlayWindowFlags();
        OverlayWindowScope consoleWindowScope(consoleWindowConfig);
        if (!consoleWindowScope.IsContentVisible())
        {
            return;
        }

        const OverlayPanelRenderResult panelResult = RenderOverlayConsole(
            context->panelConfig,
            context->metrics,
            m_renderState.overlayOpacity,
            m_renderState.layoutScale,
            m_renderState.layoutPresetIndex,
            m_interactionState.dragInteractionActive,
            GetLayoutPresetLabels(),
            GetLayoutPresetCount(),
            BuildCustomPanelState());
        ApplyOverlayPanelResult(
            panelResult,
            m_interactionHandlers,
            m_interactionState.dragInteractionActive,
            m_renderState.layoutPresetIndex,
            m_renderState.layoutScale,
            m_renderState.overlayOpacity,
            m_interactionState.customEditMode,
            m_interactionState.customPaletteIndex,
            m_interactionState.customTargetRowIndex,
            m_interactionState.customIncludeMouse,
            m_interactionState.customPresetFileIndex,
            m_interactionState.customPresetNameBuffer.data(),
            m_interactionState.customPresetNameBuffer.size(),
            m_interactionState.customStatusMessage);
        if (panelResult.layoutPresetChanged || panelResult.layoutScaleChanged ||
            panelResult.customAddRequested || panelResult.customResetRequested ||
            panelResult.customAddRowRequested || panelResult.customRemoveRowRequested ||
            panelResult.customIncludeMouseChanged ||
            panelResult.customLoadPresetRequested || panelResult.customSaveAsRequested ||
            panelResult.customDuplicateRequested || panelResult.customRenameRequested ||
            panelResult.customDeleteRequested)
        {
            InvalidateRenderContext();
            // 关键参数变化后在同一帧刷新上下文，避免后续窗口尺寸和内容不同步。
            context = &GetRenderContext();
        }
    }

    OverlayWindowConfig keyStatesWindowConfig{};
    keyStatesWindowConfig.title = kKeyStatesWindowTitle;
    keyStatesWindowConfig.position = context->keyStatesWindowPos;
    keyStatesWindowConfig.preferredSize = context->windowSizes.keyStatesSize;
    keyStatesWindowConfig.overlayOpacity = m_renderState.overlayOpacity;
    keyStatesWindowConfig.windowFlags = GetOverlayWindowFlags();
    OverlayWindowScope keyStatesWindowScope(keyStatesWindowConfig);
    if (!keyStatesWindowScope.IsContentVisible())
    {
        return;
    }

    m_interactionState.keyLayoutEditState.enabled =
        m_interactionState.customEditMode && IsCustomLayoutPreset(m_renderState.layoutPresetIndex);
    if (!m_interactionState.keyLayoutEditState.enabled)
    {
        m_interactionState.keyLayoutEditState.dragFromRow = -1;
        m_interactionState.keyLayoutEditState.dragFromIndex = -1;
    }
    RenderOverlayKeyStates(
        inputService,
        uiConfig,
        context->metrics,
        context->rowSet,
        m_keyGlowEffects,
        &m_interactionState.keyLayoutEditState);
    if (m_interactionState.keyLayoutEditState.command.moveRequested ||
        m_interactionState.keyLayoutEditState.command.removeRequested)
    {
        ApplyCustomLayoutEditCommand(m_interactionState.keyLayoutEditState.command);
        InvalidateRenderContext();
    }
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
    if (m_renderState.layoutScale == clampedScale)
    {
        return;
    }
    m_renderState.layoutScale = clampedScale;
    InvalidateRenderContext();
}

OverlayPanelCustomLayoutState OverlayUI::BuildCustomPanelState()
{
    OverlayPanelCustomLayoutState state{};
    state.isCustomPreset = IsCustomLayoutPreset(m_renderState.layoutPresetIndex);
    state.editMode = m_interactionState.customEditMode;
    state.paletteIndex = m_interactionState.customPaletteIndex;
    state.targetRowIndex = m_interactionState.customTargetRowIndex;
    state.includeMouse = m_interactionState.customIncludeMouse;
    if (state.isCustomPreset)
    {
        state.includeMouse = GetCustomIncludeMouse();
        state.paletteLabels = GetCustomPaletteKeyLabels();
        state.paletteCount = GetCustomPaletteKeyCount();
        state.rowLabels = GetCustomEditableRowLabels();
        state.rowCount = GetCustomEditableRowCount();
        if (state.paletteCount > 0)
        {
            state.paletteIndex = (std::clamp)(state.paletteIndex, 0, state.paletteCount - 1);
        }
        if (state.rowCount > 0)
        {
            state.targetRowIndex = (std::clamp)(state.targetRowIndex, 0, state.rowCount - 1);
        }

        state.presetFileCount = GetCustomPresetFileCount();
        state.presetFileLabels = GetCustomPresetFileLabels();
        if (state.presetFileCount > 0)
        {
            state.presetFileIndex = (std::clamp)(m_interactionState.customPresetFileIndex, 0, state.presetFileCount - 1);
            m_interactionState.customPresetFileIndex = state.presetFileIndex;
        }
        else
        {
            state.presetFileIndex = 0;
            m_interactionState.customPresetFileIndex = 0;
        }
        state.presetNameBuffer = m_interactionState.customPresetNameBuffer.data();
        state.presetNameBufferSize = static_cast<int>(m_interactionState.customPresetNameBuffer.size());
        state.statusMessage = m_interactionState.customStatusMessage.c_str();
    }
    return state;
}

const OverlayRenderContext& OverlayUI::GetRenderContext() const
{
    if (m_renderContextDirty)
    {
        m_renderContextCache = BuildOverlayRenderContext(
            m_renderState.layoutScale,
            m_renderState.consoleHidden,
            m_renderState.layoutPresetIndex,
            GetOverlayUIConfig());
        m_renderContextDirty = false;
    }
    return m_renderContextCache;
}

void OverlayUI::InvalidateRenderContext()
{
    m_renderContextDirty = true;
}

void OverlayUI::SetConsoleHidden(bool hidden)
{
    if (m_renderState.consoleHidden == hidden)
    {
        return;
    }

    const OverlayRenderContext previousContext = GetRenderContext();
    m_renderState.consoleHidden = hidden;
    InvalidateRenderContext();
    const OverlayRenderContext currentContext = GetRenderContext();

    const int deltaX = static_cast<int>(std::lround(previousContext.keyStatesWindowPos.x - currentContext.keyStatesWindowPos.x));
    const int deltaY = static_cast<int>(std::lround(previousContext.keyStatesWindowPos.y - currentContext.keyStatesWindowPos.y));
    if ((deltaX != 0 || deltaY != 0) && m_interactionHandlers.dragRequestHandler != nullptr)
    {
        m_interactionHandlers.dragRequestHandler(m_interactionHandlers.dragRequestContext, deltaX, deltaY);
    }
}

void OverlayUI::UpdateConsoleCommandState(const InputService& inputService)
{
    const OverlayConsoleCommandAction action = m_consoleCommandTracker.Update(inputService);
    if (action == OverlayConsoleCommandAction::HideConsole && !m_renderState.consoleHidden)
    {
        SetConsoleHidden(true);
        return;
    }

    if (action == OverlayConsoleCommandAction::ShowConsole && m_renderState.consoleHidden)
    {
        SetConsoleHidden(false);
    }
}
} // namespace keyviz
