#include "OverlayKeyRenderer.h"

#include <algorithm>
#include <vector>

#include "imgui.h"

#ifdef max
#undef max
#endif

#ifdef min
#undef min
#endif

namespace keyviz
{
namespace
{
ImVec4 LerpColor(const ImVec4& a, const ImVec4& b, float t)
{
    t = std::clamp(t, 0.0f, 1.0f);
    return ImVec4(
        a.x + (b.x - a.x) * t,
        a.y + (b.y - a.y) * t,
        a.z + (b.z - a.z) * t,
        a.w + (b.w - a.w) * t);
}

float GetKeyAnimationValue(const InputService& inputService, const GlowEffect* glow, std::uint32_t keyCode)
{
    float animationValue = 0.0f;
    if (const KeyState* state = inputService.TryGetKeyState(keyCode))
    {
        animationValue = state->animationProgress;
    }

    const float glowValue = glow != nullptr ? glow->GetCurrentIntensity() : 0.0f;
    return (std::max)(animationValue, glowValue);
}

void DrawKeyCap(const char* label, float animationValue, bool wasPressed, float keyWidth, const LayoutMetrics& metrics)
{
    ImGui::PushID(label);
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 8.0f * metrics.scale);
    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f * metrics.scale);

    const ImVec4 transparentFill = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
    const ImVec4 fillBaseColor = ImVec4(0.20f, 0.58f, 0.96f, 0.40f);
    const ImVec4 flashColor = ImVec4(0.96f, 0.74f, 0.24f, 1.0f);
    const ImVec4 borderColor = ImVec4(0.92f, 0.95f, 1.00f, 0.92f);
    const ImVec4 textColor = ImVec4(0.95f, 0.97f, 1.00f, 0.98f);

    ImVec4 fillColor = LerpColor(transparentFill, fillBaseColor, animationValue);
    if (wasPressed)
    {
        fillColor = LerpColor(fillColor, flashColor, 0.35f);
    }

    ImGui::PushStyleColor(ImGuiCol_Button, fillColor);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, fillColor);
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, fillColor);
    ImGui::PushStyleColor(ImGuiCol_Text, textColor);
    ImGui::PushStyleColor(ImGuiCol_Border, borderColor);

    const ImVec2 buttonSize = ImVec2(keyWidth, GetKeyVisualHeight(metrics));
    ImGui::Button(label, buttonSize);

    ImGui::PopStyleColor(5);
    ImGui::PopStyleVar(2);
    ImGui::PopID();
}
} // namespace

void DrawKeyboardCluster(
    const InputService& inputService,
    KeyRowSet rowSet,
    const LayoutMetrics& metrics,
    const std::unordered_map<std::uint32_t, GlowEffect>& keyGlowEffects,
    float sectionInset,
    float sectionGap,
    const char* sectionLabel,
    const char* footerText)
{
    ImGui::Indent(sectionInset);
    ImGui::TextUnformatted(sectionLabel);
    ImGui::Dummy(ImVec2(0.0f, sectionGap * metrics.scale));

    std::vector<float> rowWidths(rowSet.count, 0.0f);
    float clusterWidthMax = 0.0f;
    for (std::size_t rowIndex = 0; rowIndex < rowSet.count; ++rowIndex)
    {
        rowWidths[rowIndex] = MeasureRowWidth(rowSet.rows[rowIndex].keys, rowSet.rows[rowIndex].keyCount, metrics);
        clusterWidthMax = (std::max)(clusterWidthMax, rowWidths[rowIndex]);
    }

    const float clusterContentHeight = MeasureClusterContentHeight(metrics, rowSet);

    const ImVec2 clusterOrigin = ImGui::GetCursorScreenPos();
    const ImVec2 clusterMin = ImVec2(clusterOrigin.x - metrics.rowPaddingX, clusterOrigin.y - metrics.rowPaddingY);
    const ImVec2 clusterMax = ImVec2(
        clusterOrigin.x + clusterWidthMax + metrics.rowPaddingX,
        clusterOrigin.y + clusterContentHeight + metrics.rowPaddingY);

    ImDrawList* drawList = ImGui::GetWindowDrawList();
    drawList->AddRectFilled(
        clusterMin,
        clusterMax,
        ImGui::GetColorU32(ImVec4(0.08f, 0.09f, 0.11f, 0.58f)),
        16.0f * metrics.scale);
    drawList->AddRect(
        clusterMin,
        clusterMax,
        ImGui::GetColorU32(ImVec4(0.38f, 0.56f, 0.86f, 0.26f)),
        16.0f * metrics.scale,
        0,
        1.0f * metrics.scale);

    auto drawRow = [&](const KeyBinding* keys, std::size_t keyCount, float rowY, float rowWidth)
    {
        const float rowStartX = clusterOrigin.x + (clusterWidthMax - rowWidth) * 0.5f;
        ImGui::SetCursorScreenPos(ImVec2(rowStartX, clusterOrigin.y + rowY));

        for (std::size_t i = 0; i < keyCount; ++i)
        {
            const KeyBinding& binding = keys[i];
            const GlowEffect* glow = nullptr;
            const auto glowIt = keyGlowEffects.find(binding.keyCode);
            if (glowIt != keyGlowEffects.end())
            {
                glow = &glowIt->second;
            }

            const float animationValue = GetKeyAnimationValue(inputService, glow, binding.keyCode);
            const bool wasPressed = inputService.WasPressedThisFrame(binding.keyCode);
            ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 1.0f);
            DrawKeyCap(binding.label, animationValue, wasPressed, GetKeyVisualWidth(binding, metrics), metrics);
            ImGui::PopStyleVar();

            if (i + 1U < keyCount)
            {
                ImGui::SameLine(0.0f, metrics.keySpacing);
            }
        }
    };

    const float rowStepY = GetKeyVisualHeight(metrics) + metrics.rowSpacing;
    for (std::size_t rowIndex = 0; rowIndex < rowSet.count; ++rowIndex)
    {
        drawRow(
            rowSet.rows[rowIndex].keys,
            rowSet.rows[rowIndex].keyCount,
            static_cast<float>(rowIndex) * rowStepY,
            rowWidths[rowIndex]);
    }

    ImGui::Dummy(ImVec2(clusterWidthMax, clusterContentHeight));
    ImGui::Spacing();
    ImGui::TextDisabled(footerText);
    ImGui::Unindent(sectionInset);
}
} // namespace keyviz
