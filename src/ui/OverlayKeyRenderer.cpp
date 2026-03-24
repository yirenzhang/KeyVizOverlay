#include "OverlayKeyRenderer.h"

#include <algorithm>
#include <array>
#include <cfloat>
#include <cstring>
#include <cmath>
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
struct FontSelection
{
    ImFont* font = nullptr;
};

constexpr std::array<float, 6> kOverlayFontSizes =
{
    12.0f, 16.0f, 20.0f, 24.0f, 28.0f, 32.0f
};

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

FontSelection SelectNearestFont(float targetSize)
{
    ImFontAtlas* atlas = ImGui::GetIO().Fonts;
    if (atlas == nullptr || atlas->Fonts.empty())
    {
        return FontSelection{ ImGui::GetFont() };
    }

    const std::size_t fontCount = atlas->Fonts.size();
    const std::size_t keyFontCount = kOverlayFontSizes.size();
    if (fontCount <= keyFontCount)
    {
        return FontSelection{ atlas->Fonts[fontCount - 1U] };
    }
    const std::size_t keyFontStart = fontCount - keyFontCount;

    std::size_t bestIndex = 0U;
    float bestDistance = std::abs(kOverlayFontSizes[0] - targetSize);
    for (std::size_t i = 1; i < kOverlayFontSizes.size(); ++i)
    {
        const float distance = std::abs(kOverlayFontSizes[i] - targetSize);
        if (distance < bestDistance)
        {
            bestDistance = distance;
            bestIndex = i;
        }
    }

    const std::size_t atlasIndex = keyFontStart + bestIndex;
    return FontSelection{ atlas->Fonts[atlasIndex] };
}

bool IsMouseVisualRow(const KeyBinding* keys, std::size_t keyCount)
{
    return keyCount == 3 &&
        keys[0].keyCode == VK_LBUTTON &&
        keys[1].keyCode == VK_RBUTTON &&
        keys[2].keyCode == VK_MBUTTON;
}

float ComputeRowsHeight(std::size_t rowCount, const LayoutMetrics& metrics)
{
    if (rowCount == 0U)
    {
        return 0.0f;
    }

    const float count = static_cast<float>(rowCount);
    return GetKeyVisualHeight(metrics) * count + metrics.rowSpacing * (count - 1.0f);
}

float GetMouseVisualWidth(const LayoutMetrics& metrics)
{
    return metrics.keyWidth * 1.95f;
}

float GetMouseVisualHeight(const LayoutMetrics& metrics, float keyboardHeight)
{
    const float minimumMouseHeight = GetKeyVisualHeight(metrics) * 2.35f;
    return (std::max)(keyboardHeight, minimumMouseHeight);
}

float LerpFloat(float a, float b, float t)
{
    t = std::clamp(t, 0.0f, 1.0f);
    return a + (b - a) * t;
}

float SnapPixel(float value)
{
    return std::floor(value + 0.5f);
}

ImVec2 SnapVec2(const ImVec2& value)
{
    return ImVec2(SnapPixel(value.x), SnapPixel(value.y));
}

float GetSegmentXAtY(const ImVec2& a, const ImVec2& b, float y)
{
    const float dy = b.y - a.y;
    if (std::abs(dy) < 1e-5f)
    {
        return a.x;
    }

    const float t = (y - a.y) / dy;
    return LerpFloat(a.x, b.x, t);
}

void DrawMouseShape(
    const InputService& inputService,
    const std::unordered_map<std::uint32_t, GlowEffect>& keyGlowEffects,
    const KeyBinding* keys,
    const ImVec2& mouseSize,
    const LayoutMetrics& metrics)
{
    const ImVec2 mousePos = ImGui::GetCursorScreenPos();
    const ImVec2 mouseMin = mousePos;
    const ImVec2 mouseMax(mousePos.x + mouseSize.x, mousePos.y + mouseSize.y);
    const float upperSplitY = mouseMin.y + mouseSize.y * 0.42f;
    const float sideInset = mouseSize.x * 0.15f;
    const float taperInset = mouseSize.x * 0.08f;
    const float lineThickness = (std::max)(1.0f, SnapPixel(1.0f * metrics.scale));

    ImDrawList* drawList = ImGui::GetWindowDrawList();
    ImVec2 shellPoints[6] =
    {
        ImVec2(mouseMin.x + sideInset, mouseMin.y),
        ImVec2(mouseMax.x - sideInset, mouseMin.y),
        ImVec2(mouseMax.x, upperSplitY),
        ImVec2(mouseMax.x - taperInset, mouseMax.y),
        ImVec2(mouseMin.x + taperInset, mouseMax.y),
        ImVec2(mouseMin.x, upperSplitY),
    };
    for (ImVec2& point : shellPoints)
    {
        point = SnapVec2(point);
    }
    drawList->AddPolyline(shellPoints, 6, ImGui::GetColorU32(ImVec4(0.92f, 0.95f, 1.00f, 0.92f)), true, lineThickness);

    const float topY = mouseMin.y + lineThickness;
    const float bottomY = upperSplitY - lineThickness;
    const float topInnerLeftX = mouseMin.x + sideInset + lineThickness;
    const float topInnerRightX = mouseMax.x - sideInset - lineThickness;
    const float topInnerWidth = topInnerRightX - topInnerLeftX;
    const float middleWidth = topInnerWidth * 0.22f;
    const float sideWidth = (topInnerWidth - middleWidth) * 0.5f;
    const float leftDividerX = topInnerLeftX + sideWidth;
    const float rightDividerX = leftDividerX + middleWidth;

    // 沿外壳斜边求交，保证特效填充与鼠标边线严丝合缝。
    const ImVec2 leftEdgeTop(mouseMin.x + sideInset, mouseMin.y);
    const ImVec2 leftEdgeBottom(mouseMin.x, upperSplitY);
    const ImVec2 rightEdgeTop(mouseMax.x - sideInset, mouseMin.y);
    const ImVec2 rightEdgeBottom(mouseMax.x, upperSplitY);
    const float leftTopX = GetSegmentXAtY(leftEdgeTop, leftEdgeBottom, topY) + lineThickness;
    const float leftBottomX = GetSegmentXAtY(leftEdgeTop, leftEdgeBottom, bottomY) + lineThickness;
    const float rightTopX = GetSegmentXAtY(rightEdgeTop, rightEdgeBottom, topY) - lineThickness;
    const float rightBottomX = GetSegmentXAtY(rightEdgeTop, rightEdgeBottom, bottomY) - lineThickness;

    const ImVec2 leftTopLeft(leftTopX, topY);
    const ImVec2 leftTopRight(leftDividerX - lineThickness, topY);
    const ImVec2 leftBottomRight(leftDividerX - lineThickness, bottomY);
    const ImVec2 leftBottomLeft(leftBottomX, bottomY);

    const ImVec2 middleTopLeft(leftDividerX + lineThickness, topY);
    const ImVec2 middleTopRight(rightDividerX - lineThickness, topY);
    const ImVec2 middleBottomRight(rightDividerX - lineThickness, bottomY);
    const ImVec2 middleBottomLeft(leftDividerX + lineThickness, bottomY);

    const ImVec2 rightTopLeft(rightDividerX + lineThickness, topY);
    const ImVec2 rightTopRight(rightTopX, topY);
    const ImVec2 rightBottomRight(rightBottomX, bottomY);
    const ImVec2 rightBottomLeft(rightDividerX + lineThickness, bottomY);

    const std::uint32_t leftKey = keys[0].keyCode;
    const std::uint32_t rightKey = keys[1].keyCode;
    const std::uint32_t middleKey = keys[2].keyCode;

    auto drawButtonRegion = [&](const ImVec2* points, std::uint32_t keyCode) {
        const GlowEffect* glow = FindGlowEffect(keyGlowEffects, keyCode);
        const float animationValue = GetKeyAnimationValue(inputService, glow, keyCode);
        const bool wasPressed = inputService.WasPressedThisFrame(keyCode);
        ImVec4 fillColor = LerpColor(ImVec4(0.0f, 0.0f, 0.0f, 0.0f), ImVec4(0.00f, 0.78f, 1.00f, 0.58f), animationValue);
        if (wasPressed)
        {
            fillColor = LerpColor(fillColor, ImVec4(0.96f, 0.74f, 0.24f, 1.0f), 0.45f);
        }

        drawList->AddConvexPolyFilled(points, 4, ImGui::GetColorU32(fillColor));
    };

    ImVec2 leftPoints[4] = { leftTopLeft, leftTopRight, leftBottomRight, leftBottomLeft };
    ImVec2 middlePoints[4] = { middleTopLeft, middleTopRight, middleBottomRight, middleBottomLeft };
    ImVec2 rightPoints[4] = { rightTopLeft, rightTopRight, rightBottomRight, rightBottomLeft };
    for (ImVec2& point : leftPoints)
    {
        point = SnapVec2(point);
    }
    for (ImVec2& point : middlePoints)
    {
        point = SnapVec2(point);
    }
    for (ImVec2& point : rightPoints)
    {
        point = SnapVec2(point);
    }
    drawButtonRegion(leftPoints, leftKey);
    drawButtonRegion(middlePoints, middleKey);
    drawButtonRegion(rightPoints, rightKey);

    drawList->AddLine(
        SnapVec2(ImVec2(leftDividerX, topY)),
        SnapVec2(ImVec2(leftDividerX, bottomY)),
        ImGui::GetColorU32(ImVec4(0.92f, 0.95f, 1.00f, 0.55f)),
        lineThickness);
    drawList->AddLine(
        SnapVec2(ImVec2(rightDividerX, topY)),
        SnapVec2(ImVec2(rightDividerX, bottomY)),
        ImGui::GetColorU32(ImVec4(0.92f, 0.95f, 1.00f, 0.55f)),
        lineThickness);
    drawList->AddLine(
        SnapVec2(ImVec2(leftBottomX, bottomY)),
        SnapVec2(ImVec2(rightBottomX, bottomY)),
        ImGui::GetColorU32(ImVec4(0.92f, 0.95f, 1.00f, 0.42f)),
        lineThickness);

    const float wheelCenterX = mouseMin.x + sideWidth + middleWidth * 0.5f;
    const float wheelTopY = LerpFloat(topY, bottomY, 0.20f);
    const float wheelBottomY = LerpFloat(topY, bottomY, 0.72f);
    drawList->AddLine(
        SnapVec2(ImVec2(wheelCenterX, wheelTopY)),
        SnapVec2(ImVec2(wheelCenterX, wheelBottomY)),
        ImGui::GetColorU32(ImVec4(0.92f, 0.95f, 1.00f, 0.85f)),
        (std::max)(1.0f, SnapPixel(2.0f * metrics.scale)));

}

void DrawKeyCap(const char* label, float animationValue, bool wasPressed, float keyWidth, const LayoutMetrics& metrics)
{
    ImGui::PushID(label);

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

    const ImVec2 buttonSize = ImVec2(keyWidth, GetKeyVisualHeight(metrics));
    const ImVec2 keyMin = ImGui::GetCursorScreenPos();
    const ImVec2 keyMax(keyMin.x + buttonSize.x, keyMin.y + buttonSize.y);
    const float slantOffset = SnapPixel(7.0f * metrics.scale);
    ImVec2 points[4] =
    {
        ImVec2(keyMin.x + slantOffset, keyMin.y),
        ImVec2(keyMax.x, keyMin.y),
        ImVec2(keyMax.x - slantOffset, keyMax.y),
        ImVec2(keyMin.x, keyMax.y),
    };
    for (ImVec2& point : points)
    {
        point = SnapVec2(point);
    }

    ImDrawList* drawList = ImGui::GetWindowDrawList();
    drawList->AddConvexPolyFilled(points, 4, ImGui::GetColorU32(fillColor));
    drawList->AddPolyline(points, 4, ImGui::GetColorU32(borderColor), true, (std::max)(1.0f, SnapPixel(1.0f * metrics.scale)));

    if (label != nullptr && label[0] != '\0')
    {
        // 选择最接近目标字号的栅格字体，避免单字体连续缩放带来的模糊。
        float targetFontSize = ImGui::GetFontSize() * metrics.scale * 0.95f;
        targetFontSize = std::clamp(targetFontSize, 10.0f, 42.0f);
        const FontSelection fontSelection = SelectNearestFont(targetFontSize);
        ImFont* font = fontSelection.font != nullptr ? fontSelection.font : ImGui::GetFont();

        ImGui::PushFont(font);
        const ImVec2 textSize = ImGui::CalcTextSize(label);
        const float innerPadX = 5.0f * metrics.scale;
        const float innerPadY = 3.0f * metrics.scale;
        const ImVec2 clipMin = SnapVec2(ImVec2(keyMin.x + slantOffset + innerPadX, keyMin.y + innerPadY));
        const ImVec2 clipMax = SnapVec2(ImVec2(keyMax.x - innerPadX, keyMax.y - innerPadY));
        float textPosX = keyMin.x + (buttonSize.x - textSize.x) * 0.5f;
        // SHIFT/CTRL 在宽键帽中视觉中心略偏右，做轻微左移微调。
        if (std::strcmp(label, "SHIFT") == 0 || std::strcmp(label, "CTRL") == 0)
        {
            textPosX -= (std::max)(1.0f, std::round(metrics.scale));
        }
        textPosX = (std::max)(textPosX, clipMin.x);
        float textPosY = keyMin.y + (buttonSize.y - textSize.y) * 0.5f;
        textPosX = std::floor(textPosX + 0.5f);
        textPosY = std::floor(textPosY + 0.5f);
        drawList->PushClipRect(clipMin, clipMax, true);
        drawList->AddText(ImVec2(textPosX, textPosY), ImGui::GetColorU32(textColor), label);
        drawList->PopClipRect();
        ImGui::PopFont();
    }

    ImGui::Dummy(buttonSize);
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
    const char* sectionLabel)
{
    ImGui::Indent(sectionInset);
    ImGui::TextUnformatted(sectionLabel);
    ImGui::Dummy(ImVec2(0.0f, sectionGap * metrics.scale));

    std::vector<std::size_t> keyboardRowIndices;
    keyboardRowIndices.reserve(rowSet.count);
    std::size_t mouseRowIndex = static_cast<std::size_t>(-1);
    std::vector<float> rowWidths(rowSet.count, 0.0f);
    float keyboardWidthMax = 0.0f;
    for (std::size_t rowIndex = 0; rowIndex < rowSet.count; ++rowIndex)
    {
        const KeyRow& row = rowSet.rows[rowIndex];
        if (IsMouseVisualRow(row.keys, row.keyCount))
        {
            mouseRowIndex = rowIndex;
            continue;
        }

        rowWidths[rowIndex] = MeasureRowWidth(row.keys, row.keyCount, metrics);
        keyboardWidthMax = (std::max)(keyboardWidthMax, rowWidths[rowIndex]);
        keyboardRowIndices.push_back(rowIndex);
    }

    const bool hasMouseRow = mouseRowIndex != static_cast<std::size_t>(-1);
    const float keyboardHeight = ComputeRowsHeight(keyboardRowIndices.size(), metrics);
    const float mouseWidth = hasMouseRow ? GetMouseVisualWidth(metrics) : 0.0f;
    const float mouseHeight = hasMouseRow ? GetMouseVisualHeight(metrics, keyboardHeight) : 0.0f;
    const float keyboardMouseGap = (hasMouseRow && keyboardWidthMax > 0.0f) ? (metrics.keySpacing * 1.5f) : 0.0f;
    const float clusterWidthMax = keyboardWidthMax + keyboardMouseGap + mouseWidth;
    const float clusterContentHeight = (std::max)(keyboardHeight, mouseHeight);

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
        const float rowStartX = clusterOrigin.x + (keyboardWidthMax - rowWidth) * 0.5f;
        ImGui::SetCursorScreenPos(ImVec2(rowStartX, clusterOrigin.y + rowY));

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
    for (std::size_t visualRowIndex = 0; visualRowIndex < keyboardRowIndices.size(); ++visualRowIndex)
    {
        const std::size_t rowIndex = keyboardRowIndices[visualRowIndex];
        drawRow(
            rowSet.rows[rowIndex].keys,
            rowSet.rows[rowIndex].keyCount,
            static_cast<float>(visualRowIndex) * rowStepY,
            rowWidths[rowIndex]);
    }

    if (hasMouseRow)
    {
        const KeyRow& mouseRow = rowSet.rows[mouseRowIndex];
        const float mouseX = clusterOrigin.x + keyboardWidthMax + keyboardMouseGap;
        const float mouseY = clusterOrigin.y + (clusterContentHeight - mouseHeight) * 0.5f;
        ImGui::SetCursorScreenPos(ImVec2(mouseX, mouseY));
        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 1.0f);
        DrawMouseShape(inputService, keyGlowEffects, mouseRow.keys, ImVec2(mouseWidth, mouseHeight), metrics);
        ImGui::PopStyleVar();
    }

    ImGui::SetCursorScreenPos(clusterOrigin);
    ImGui::Dummy(ImVec2(clusterWidthMax, clusterContentHeight));
    ImGui::Unindent(sectionInset);
}
} // namespace keyviz
