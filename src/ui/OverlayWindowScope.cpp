#include "OverlayWindowScope.h"

namespace keyviz
{
OverlayWindowScope::OverlayWindowScope(const OverlayWindowConfig& config)
{
    ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f), ImGuiCond_Always);
    ImGui::SetNextWindowSize(config.preferredSize, ImGuiCond_Always);
    ImGui::SetNextWindowBgAlpha(config.overlayOpacity);

    const ImGuiStyle& style = ImGui::GetStyle();
    const float windowBorderSize = config.overlayOpacity <= 0.0f ? 0.0f : style.WindowBorderSize;
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, windowBorderSize);
    m_contentVisible = ImGui::Begin(config.title, nullptr, config.windowFlags);
    ImGui::PopStyleVar();

    ImGui::PushStyleVar(ImGuiStyleVar_Alpha, config.overlayOpacity);
    m_active = true;
}

OverlayWindowScope::~OverlayWindowScope()
{
    if (!m_active)
    {
        return;
    }

    ImGui::PopStyleVar();
    ImGui::End();
}

bool OverlayWindowScope::IsContentVisible() const
{
    return m_contentVisible;
}
} // namespace keyviz
