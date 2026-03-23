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
namespace
{
constexpr std::size_t kCommandBufferLimit = 24U;
constexpr const char* kHideCommand = "hidehide";
constexpr const char* kShowCommand = "showshow";
constexpr const char* kConsoleWindowTitle = "KeyViz Console";
constexpr const char* kKeyStatesWindowTitle = "Key states";

bool EndsWithCommand(const std::string& text, const char* suffix)
{
    const std::size_t suffixLength = std::char_traits<char>::length(suffix);
    if (text.size() < suffixLength)
    {
        return false;
    }

    return text.compare(text.size() - suffixLength, suffixLength, suffix) == 0;
}

void AppendCommandLetter(const InputService& inputService, std::uint32_t keyCode, char letter, std::string& commandBuffer)
{
    if (!inputService.WasPressedThisFrame(keyCode))
    {
        return;
    }

    commandBuffer.push_back(letter);
    if (commandBuffer.size() > kCommandBufferLimit)
    {
        commandBuffer.erase(0, commandBuffer.size() - kCommandBufferLimit);
    }
}
}

void OverlayUI::Initialize()
{
    m_keyGlowEffects.clear();
    m_consoleHidden = false;
    m_consoleCommandBuffer.clear();
    m_dragInteractionActive = false;
    InvalidateRenderContext();
}

void OverlayUI::Shutdown()
{
    m_keyGlowEffects.clear();
    m_consoleCommandBuffer.clear();
}

void OverlayUI::Update(float deltaSeconds, const InputService& inputService)
{
    const KeyRowSet rowSet = GetRowsForPreset(m_layoutPresetIndex);
    UpdateKeyGlowStates(deltaSeconds, inputService, rowSet, m_keyGlowEffects);
    UpdateConsoleCommandState(inputService);
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
    if (!m_consoleHidden)
    {
        OverlayWindowConfig consoleWindowConfig{};
        consoleWindowConfig.title = kConsoleWindowTitle;
        consoleWindowConfig.position = context.consoleWindowPos;
        consoleWindowConfig.preferredSize = context.windowSizes.consoleSize;
        consoleWindowConfig.overlayOpacity = 1.0f;
        consoleWindowConfig.windowFlags = GetOverlayWindowFlags();
        OverlayWindowScope consoleWindowScope(consoleWindowConfig);
        if (!consoleWindowScope.IsContentVisible())
        {
            return;
        }

        const OverlayPanelRenderResult panelResult = RenderOverlayConsole(
            context.panelConfig,
            context.metrics,
            m_overlayOpacity,
            m_layoutPresetIndex,
            m_dragInteractionActive,
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
            return;
        }
    }

    OverlayWindowConfig keyStatesWindowConfig{};
    keyStatesWindowConfig.title = kKeyStatesWindowTitle;
    keyStatesWindowConfig.position = context.keyStatesWindowPos;
    keyStatesWindowConfig.preferredSize = context.windowSizes.keyStatesSize;
    keyStatesWindowConfig.overlayOpacity = m_overlayOpacity;
    keyStatesWindowConfig.windowFlags = GetOverlayWindowFlags();
    OverlayWindowScope keyStatesWindowScope(keyStatesWindowConfig);
    if (!keyStatesWindowScope.IsContentVisible())
    {
        return;
    }

    RenderOverlayKeyStates(
        inputService,
        uiConfig,
        context.metrics,
        context.rowSet,
        m_keyGlowEffects,
        m_showDebugPanel);
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
            m_consoleHidden,
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

void OverlayUI::UpdateConsoleCommandState(const InputService& inputService)
{
    AppendCommandLetter(inputService, 'H', 'h', m_consoleCommandBuffer);
    AppendCommandLetter(inputService, 'I', 'i', m_consoleCommandBuffer);
    AppendCommandLetter(inputService, 'D', 'd', m_consoleCommandBuffer);
    AppendCommandLetter(inputService, 'E', 'e', m_consoleCommandBuffer);
    AppendCommandLetter(inputService, 'S', 's', m_consoleCommandBuffer);
    AppendCommandLetter(inputService, 'O', 'o', m_consoleCommandBuffer);
    AppendCommandLetter(inputService, 'W', 'w', m_consoleCommandBuffer);

    if (EndsWithCommand(m_consoleCommandBuffer, kHideCommand))
    {
        if (!m_consoleHidden)
        {
            m_consoleHidden = true;
            InvalidateRenderContext();
        }
        m_consoleCommandBuffer.clear();
        return;
    }

    if (EndsWithCommand(m_consoleCommandBuffer, kShowCommand))
    {
        if (m_consoleHidden)
        {
            m_consoleHidden = false;
            InvalidateRenderContext();
        }
        m_consoleCommandBuffer.clear();
    }
}
} // namespace keyviz
