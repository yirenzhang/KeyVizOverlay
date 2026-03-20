#pragma once

namespace keyviz
{
using OverlayExitRequestHandler = void (*)(void* context);
using OverlayDragRequestHandler = void (*)(void* context, int deltaX, int deltaY);
using OverlayDragStateRequestHandler = void (*)(void* context, bool active);

struct OverlayUIInteractionHandlers
{
    OverlayExitRequestHandler exitRequestHandler = nullptr;
    void* exitRequestContext = nullptr;
    OverlayDragRequestHandler dragRequestHandler = nullptr;
    void* dragRequestContext = nullptr;
    OverlayDragStateRequestHandler dragStateRequestHandler = nullptr;
    void* dragStateRequestContext = nullptr;
};
} // namespace keyviz
