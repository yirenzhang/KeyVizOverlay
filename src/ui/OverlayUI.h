#pragma once

#include <cstdint>
#include <unordered_map>

#include "effects/GlowEffect.h"
#include "input/InputService.h"
#include "OverlayUICallbacks.h"
#include "OverlayRenderContext.h"
#include "imgui.h"

namespace keyviz
{
class OverlayUI
{
public:
    using ExitRequestHandler = OverlayExitRequestHandler;
    using DragRequestHandler = OverlayDragRequestHandler;
    using DragStateRequestHandler = OverlayDragStateRequestHandler;

    OverlayUI() = default;

    void Initialize();
    void Shutdown();
    void Update(float deltaSeconds, const InputService& inputService);
    void Render(const InputService& inputService);
    void DrawKeyboardVisualizer(const InputService& inputService);

    ImVec2 GetPreferredWindowSize() const;
    float GetOverlayOpacity() const;
    void SetShowDebugPanel(bool show);
    void SetExitRequestHandler(ExitRequestHandler handler, void* context = nullptr);
    void SetDragRequestHandler(DragRequestHandler handler, void* context = nullptr);
    void SetDragStateRequestHandler(DragStateRequestHandler handler, void* context = nullptr);
    void SetLayoutScale(float scale);

private:
    const OverlayRenderContext& GetRenderContext() const;
    void InvalidateRenderContext();

    bool m_showDebugPanel = true;
    float m_layoutScale = 1.0f;
    float m_overlayOpacity = 0.86f;
    int m_layoutPresetIndex = 0;
    bool m_dragInteractionActive = false;
    OverlayUIInteractionHandlers m_interactionHandlers{};
    mutable bool m_renderContextDirty = true;
    mutable OverlayRenderContext m_renderContextCache{};

    std::unordered_map<std::uint32_t, GlowEffect> m_keyGlowEffects;
};
}
