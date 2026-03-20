#pragma once

#include "imgui.h"

namespace keyviz
{
struct OverlayWindowConfig
{
    const char* title = nullptr;
    ImVec2 preferredSize{};
    float overlayOpacity = 1.0f;
    ImGuiWindowFlags windowFlags = 0;
};

class OverlayWindowScope
{
public:
    explicit OverlayWindowScope(const OverlayWindowConfig& config);
    ~OverlayWindowScope();

    OverlayWindowScope(const OverlayWindowScope&) = delete;
    OverlayWindowScope& operator=(const OverlayWindowScope&) = delete;

    bool IsContentVisible() const;

private:
    bool m_active = false;
    bool m_contentVisible = false;
};
} // namespace keyviz
