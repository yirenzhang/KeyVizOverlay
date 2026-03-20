#pragma once

#include <cstdint>
#include <unordered_map>

#include "effects/GlowEffect.h"
#include "input/InputService.h"
#include "imgui.h"

namespace keyviz
{
class OverlayUI
{
public:
    using ExitRequestHandler = void (*)(void* context);
    using DragRequestHandler = void (*)(void* context, int deltaX, int deltaY);
    using DragStateRequestHandler = void (*)(void* context, bool active);

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
    bool m_showDebugPanel = true;
    float m_layoutScale = 1.0f;
    float m_overlayOpacity = 0.86f;
    int m_layoutPresetIndex = 0;
    bool m_dragInteractionActive = false;
    ExitRequestHandler m_exitRequestHandler = nullptr;
    void* m_exitRequestContext = nullptr;
    DragRequestHandler m_dragRequestHandler = nullptr;
    void* m_dragRequestContext = nullptr;
    DragStateRequestHandler m_dragStateRequestHandler = nullptr;
    void* m_dragStateRequestContext = nullptr;

    std::unordered_map<std::uint32_t, GlowEffect> m_keyGlowEffects;
};
}
