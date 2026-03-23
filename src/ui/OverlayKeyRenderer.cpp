#include "OverlayKeyRenderer.h"

#include <algorithm>
#include <vector>

#include "imgui.h"
#include <windows.h>

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

const GlowEffect* FindGlowEffect(const std::unordered_map<std::uint32_t, GlowEffect>& keyGlowEffects, std::uint32_t keyCode)
{
    const auto glowIt = keyGlowEffects.find(keyCode);
    if (glowIt == keyGlowEffects.end())
    {
        return nullptr;
    }

    return &glowIt->second;
}

bool IsMouseVisualRow(const KeyBinding* keys, std::size_t keyCount)
{
    return keyCount == 3 &&
        keys[0].keyCode == VK_LBUTTON &&
        keys[1].keyCode == VK_RBUTTON &&
        keys[2].keyCode == VK_MBUTTON;
}

void DrawMouseShape(
    const InputService& inputService,
    const std::unordered_map<std::uint32_t, GlowEffect>& keyGlowEffects,
    const KeyBinding* keys,
    float rowWidth,
    const LayoutMetrics& metrics)
{
    const float rowHeight = GetKeyVisualHeight(metrics);
    const ImVec2 mouseSize(rowWidth, rowHeight);
    const ImVec2 mousePos = ImGui::GetCursorScreenPos();
    const ImVec2 mouseMin = mousePos;
    const ImVec2 mouseMax(mousePos.x + mouseSize.x, mousePos.y + mouseSize.y);
    const float cornerRounding = rowHeight * 0.45f;

    ImDrawList* drawList = ImGui::GetWindowDrawList();
    // 鼠标默认内部保持透明，仅在按键动效阶段叠加区域填充。
    drawList->AddRect(
        mouseMin,
        mouseMax,
        ImGui::GetColorU32(ImVec4(0.92f, 0.95f, 1.00f, 0.92f)),
        cornerRounding,
        0,
        1.0f * metrics.scale);

    const float middleWidth = mouseSize.x * 0.24f;
    const float sideWidth = (mouseSize.x - middleWidth) * 0.5f;
    const float buttonHeight = mouseSize.y * 0.58f;

    const ImVec2 leftMin = mouseMin;
    const ImVec2 leftMax(mouseMin.x + sideWidth, mouseMin.y + buttonHeight);
    const ImVec2 rightMin(mouseMin.x + sideWidth + middleWidth, mouseMin.y);
    const ImVec2 rightMax(mouseMax.x, mouseMin.y + buttonHeight);
    const ImVec2 middleMin(mouseMin.x + sideWidth, mouseMin.y);
    const ImVec2 middleMax(mouseMin.x + sideWidth + middleWidth, mouseMin.y + buttonHeight);

    const std::uint32_t leftKey = keys[0].keyCode;
    const std::uint32_t rightKey = keys[1].keyCode;
    const std::uint32_t middleKey = keys[2].keyCode;

    auto drawButtonRegion = [&](const ImVec2& regionMin, const ImVec2& regionMax, std::uint32_t keyCode) {
        const GlowEffect* glow = FindGlowEffect(keyGlowEffects, keyCode);
        const float animationValue = GetKeyAnimationValue(inputService, glow, keyCode);
        const bool wasPressed = inputService.WasPressedThisFrame(keyCode);
        ImVec4 fillColor = LerpColor(ImVec4(0.0f, 0.0f, 0.0f, 0.0f), ImVec4(0.00f, 0.78f, 1.00f, 0.58f), animationValue);
        if (wasPressed)
        {
            fillColor = LerpColor(fillColor, ImVec4(0.96f, 0.74f, 0.24f, 1.0f), 0.45f);
        }

        drawList->AddRectFilled(regionMin, regionMax, ImGui::GetColorU32(fillColor), 6.0f * metrics.scale);
    };

    drawButtonRegion(leftMin, leftMax, leftKey);
    drawButtonRegion(rightMin, rightMax, rightKey);
    drawButtonRegion(middleMin, middleMax, middleKey);

    drawList->AddLine(
        ImVec2(mouseMin.x + sideWidth, mouseMin.y),
        ImVec2(mouseMin.x + sideWidth, mouseMin.y + buttonHeight),
        ImGui::GetColorU32(ImVec4(0.92f, 0.95f, 1.00f, 0.55f)),
        1.0f * metrics.scale);
    drawList->AddLine(
        ImVec2(mouseMin.x + sideWidth + middleWidth, mouseMin.y),
        ImVec2(mouseMin.x + sideWidth + middleWidth, mouseMin.y + buttonHeight),
        ImGui::GetColorU32(ImVec4(0.92f, 0.95f, 1.00f, 0.55f)),
        1.0f * metrics.scale);
    drawList->AddLine(
        ImVec2(mouseMin.x, mouseMin.y + buttonHeight),
        ImVec2(mouseMax.x, mouseMin.y + buttonHeight),
        ImGui::GetColorU32(ImVec4(0.92f, 0.95f, 1.00f, 0.42f)),
        1.0f * metrics.scale);

    const float wheelCenterX = mouseMin.x + sideWidth + middleWidth * 0.5f;
    drawList->AddLine(
        ImVec2(wheelCenterX, mouseMin.y + buttonHeight * 0.22f),
        ImVec2(wheelCenterX, mouseMin.y + buttonHeight * 0.78f),
        ImGui::GetColorU32(ImVec4(0.92f, 0.95f, 1.00f, 0.85f)),
        2.0f * metrics.scale);

    ImGui::Dummy(mouseSize);
}

void DrawKeyCap(const char* label, float animationValue, bool wasPressed, float keyWidth, const LayoutMetrics& metrics)
{
    ImGui::PushID(label);
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 8.0f * metrics.scale);
    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f * metrics.scale);

    const ImVec4 transparentFill = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
    const ImVec4 fillBaseColor = ImVec4(0.00f, 0.78f, 1.00f, 0.58f);
    const ImVec4 flashColor = ImVec4(0.96f, 0.74f, 0.24f, 1.0f);
    const ImVec4 borderColor = ImVec4(0.92f, 0.95f, 1.00f, 0.92f);
    const ImVec4 textColor = ImVec4(0.95f, 0.97f, 1.00f, 0.98f);

    ImVec4 fillColor = LerpColor(transparentFill, fillBaseColor, animationValue);
    if (wasPressed)
    {
        fillColor = LerpColor(fillColor, flashColor, 0.45f);
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

        if (IsMouseVisualRow(keys, keyCount))
        {
            // 鼠标造型保持完全可见，不受窗口不透明度滑条影响。
            ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 1.0f);
            DrawMouseShape(inputService, keyGlowEffects, keys, rowWidth, metrics);
            ImGui::PopStyleVar();
            return;
        }

        for (std::size_t i = 0; i < keyCount; ++i)
        {
            const KeyBinding& binding = keys[i];
            const GlowEffect* glow = FindGlowEffect(keyGlowEffects, binding.keyCode);

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
