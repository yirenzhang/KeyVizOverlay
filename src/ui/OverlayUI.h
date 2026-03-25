#pragma once

#include <array>
#include <cstdint>
#include <string>
#include <unordered_map>

#include "effects/GlowEffect.h"
#include "input/InputService.h"
#include "OverlayConsoleCommandTracker.h"
#include "OverlayPanelRenderer.h"
#include "OverlayKeyRenderer.h"
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

    ImVec2 GetPreferredWindowSize() const;
    bool IsConsoleHidden() const;
    void SetExitRequestHandler(ExitRequestHandler handler, void* context = nullptr);
    void SetDragRequestHandler(DragRequestHandler handler, void* context = nullptr);
    void SetDragStateRequestHandler(DragStateRequestHandler handler, void* context = nullptr);

private:
    struct OverlayRenderState
    {
        float layoutScale = 1.0f;
        float overlayOpacity = 0.86f;
        int layoutPresetIndex = 0;
        bool consoleHidden = false;
    };

    struct OverlayInteractionState
    {
        bool dragInteractionActive = false;
        bool customEditMode = false;
        bool customIncludeMouse = true;
        int customPaletteIndex = 0;
        int customTargetRowIndex = 0;
        int customPresetFileIndex = 0;
        std::array<char, 64> customPresetNameBuffer{};
        std::string customStatusMessage{};
        OverlayKeyLayoutEditState keyLayoutEditState{};
    };

    const OverlayRenderContext& GetRenderContext() const;
    void InvalidateRenderContext();
    void SetConsoleHidden(bool hidden);
    void UpdateConsoleCommandState(const InputService& inputService);
    OverlayPanelCustomLayoutState BuildCustomPanelState();

    OverlayRenderState m_renderState{};
    OverlayInteractionState m_interactionState{};
    OverlayUIInteractionHandlers m_interactionHandlers{};
    OverlayConsoleCommandTracker m_consoleCommandTracker{};
    mutable bool m_renderContextDirty = true;
    mutable OverlayRenderContext m_renderContextCache{};

    std::unordered_map<std::uint32_t, GlowEffect> m_keyGlowEffects;
};
}
