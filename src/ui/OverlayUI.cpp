#include "OverlayUI.h"

#include <algorithm>
#include <array>
#include <cstddef>

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
struct KeyBinding
{
    const char* label;
    std::uint32_t keyCode;
    float widthScale;
};

struct KeyRow
{
    const KeyBinding* keys;
    std::size_t keyCount;
};

struct LayoutMetrics
{
    float scale = 1.0f;
    float keyWidth = 54.0f;
    float keyHeight = 34.0f;
    float keySpacing = 8.0f;
    float rowSpacing = 16.0f;
    float rowPaddingX = 14.0f;
    float rowPaddingY = 12.0f;
    float dragBarHeight = 18.0f;
};

constexpr KeyBinding kTopRowKeys[] =
{
    { "Tab", VK_TAB, 1.10f },
    { "Q", 'Q', 1.00f },
    { "W", 'W', 1.00f },
    { "E", 'E', 1.00f },
    { "R", 'R', 1.00f },
};

constexpr KeyBinding kMiddleRowKeys[] =
{
    { "Shift", VK_SHIFT, 1.35f },
    { "A", 'A', 1.00f },
    { "S", 'S', 1.00f },
    { "D", 'D', 1.00f },
    { "F", 'F', 1.00f },
};

constexpr KeyBinding kBottomRowKeys[] =
{
    { "Ctrl", VK_CONTROL, 1.25f },
    { "Space", VK_SPACE, 3.20f },
    { "Alt", VK_MENU, 1.20f },
};

constexpr KeyBinding kActionRowKeys[] =
{
    { "Z", 'Z', 1.00f },
    { "X", 'X', 1.00f },
    { "C", 'C', 1.00f },
    { "V", 'V', 1.00f },
    { "T", 'T', 1.00f },
    { "G", 'G', 1.00f },
};

constexpr KeyBinding kNumberRowKeys[] =
{
    { "1", '1', 1.00f },
    { "2", '2', 1.00f },
    { "3", '3', 1.00f },
    { "4", '4', 1.00f },
    { "5", '5', 1.00f },
};

constexpr const char* kLayoutPresetLabels[] =
{
    "Compact",
    "Keyboard Only",
    "Keyboard + Mouse",
};

constexpr std::size_t kTopRowKeyCount = sizeof(kTopRowKeys) / sizeof(kTopRowKeys[0]);
constexpr std::size_t kMiddleRowKeyCount = sizeof(kMiddleRowKeys) / sizeof(kMiddleRowKeys[0]);
constexpr std::size_t kBottomRowKeyCount = sizeof(kBottomRowKeys) / sizeof(kBottomRowKeys[0]);
constexpr std::size_t kActionRowKeyCount = sizeof(kActionRowKeys) / sizeof(kActionRowKeys[0]);
constexpr std::size_t kNumberRowKeyCount = sizeof(kNumberRowKeys) / sizeof(kNumberRowKeys[0]);
constexpr std::size_t kLayoutPresetCount = sizeof(kLayoutPresetLabels) / sizeof(kLayoutPresetLabels[0]);

constexpr std::array<KeyRow, 5> kTrackedRows =
{
    KeyRow{ kTopRowKeys, kTopRowKeyCount },
    KeyRow{ kMiddleRowKeys, kMiddleRowKeyCount },
    KeyRow{ kBottomRowKeys, kBottomRowKeyCount },
    KeyRow{ kActionRowKeys, kActionRowKeyCount },
    KeyRow{ kNumberRowKeys, kNumberRowKeyCount },
};

constexpr const char* kOverlayTitle = "KeyViz Overlay";
constexpr const char* kDragButtonLabel = "Drag to move";
constexpr const char* kExitButtonLabel = "Exit";
constexpr const char* kLayoutPresetLabel = "Layout preset";
constexpr const char* kOpacityLabel = "Opacity";
constexpr const char* kDebugHintText = "Hold a tracked key to animate the glow.";
constexpr const char* kDebugTodoText = "TODO: Expand to the full keyboard layout later.";
constexpr const char* kKeyStatesLabel = "Key states";
constexpr const char* kFuturePlanText = "TODO: Full keyboard layout and richer effect layers will be added later.";

constexpr float kBaseKeyWidth = 54.0f;
constexpr float kBaseKeyHeight = 34.0f;
constexpr float kBaseKeySpacing = 8.0f;
constexpr float kBaseRowSpacing = 16.0f;
constexpr float kBaseRowPaddingX = 14.0f;
constexpr float kBaseRowPaddingY = 12.0f;
constexpr float kBaseDragBarHeight = 18.0f;
constexpr float kMinLayoutScale = 0.7f;
constexpr float kMaxLayoutScale = 1.8f;
constexpr float kSectionGap = 16.0f;
constexpr float kKeyStatesSectionInset = 16.0f;
constexpr float kLayoutComboWidth = 164.0f;
constexpr float kOpacitySliderWidth = 150.0f;
constexpr float kMinimumWindowWidth = 420.0f;
constexpr float kMinimumWindowHeight = 240.0f;

LayoutMetrics BuildLayoutMetrics(float scale)
{
    scale = std::clamp(scale, kMinLayoutScale, kMaxLayoutScale);

    LayoutMetrics metrics{};
    metrics.scale = scale;
    metrics.keyWidth = kBaseKeyWidth * scale;
    metrics.keyHeight = kBaseKeyHeight * scale;
    metrics.keySpacing = kBaseKeySpacing * scale;
    metrics.rowSpacing = kBaseRowSpacing * scale;
    metrics.rowPaddingX = kBaseRowPaddingX * scale;
    metrics.rowPaddingY = kBaseRowPaddingY * scale;
    metrics.dragBarHeight = kBaseDragBarHeight * scale;
    return metrics;
}

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

float GetKeyWidth(const KeyBinding& binding, const LayoutMetrics& metrics)
{
    return metrics.keyWidth * binding.widthScale;
}

float GetKeyVisualWidth(const KeyBinding& binding, const LayoutMetrics& metrics)
{
    return GetKeyWidth(binding, metrics) + 8.0f * metrics.scale;
}

float GetKeyVisualHeight(const LayoutMetrics& metrics)
{
    return metrics.keyHeight + 4.0f * metrics.scale;
}

float MeasureRowWidth(const KeyBinding* keys, std::size_t keyCount, const LayoutMetrics& metrics)
{
    float rowWidth = 0.0f;
    for (std::size_t i = 0; i < keyCount; ++i)
    {
        rowWidth += GetKeyVisualWidth(keys[i], metrics);
        if (i + 1U < keyCount)
        {
            rowWidth += metrics.keySpacing;
        }
    }

    return rowWidth;
}

float MeasureMaxRowWidth(const LayoutMetrics& metrics)
{
    float maxWidth = 0.0f;
    for (const KeyRow& row : kTrackedRows)
    {
        maxWidth = (std::max)(maxWidth, MeasureRowWidth(row.keys, row.keyCount, metrics));
    }
    return maxWidth;
}

float MeasureClusterContentHeight(const LayoutMetrics& metrics)
{
    const float rowCount = static_cast<float>(kTrackedRows.size());
    return (GetKeyVisualHeight(metrics) * rowCount) + (metrics.rowSpacing * (rowCount - 1.0f));
}

float MeasureClusterBackgroundHeight(const LayoutMetrics& metrics)
{
    return MeasureClusterContentHeight(metrics) + (metrics.rowPaddingY * 2.0f);
}

float MeasureButtonWidth(const char* label, const LayoutMetrics& metrics)
{
    const ImGuiStyle& style = ImGui::GetStyle();
    return ImGui::CalcTextSize(label).x + style.FramePadding.x * 2.0f + 18.0f * metrics.scale;
}

float MeasureHeaderRowWidth(const LayoutMetrics& metrics)
{
    const ImGuiStyle& style = ImGui::GetStyle();
    const float titleWidth = ImGui::CalcTextSize(kOverlayTitle).x;
    const float dragButtonWidth = MeasureButtonWidth(kDragButtonLabel, metrics);
    const float exitButtonWidth = MeasureButtonWidth(kExitButtonLabel, metrics);
    return titleWidth + style.ItemSpacing.x + dragButtonWidth + style.ItemSpacing.x + exitButtonWidth;
}

float MeasureControlsRowWidth(const LayoutMetrics& metrics)
{
    const ImGuiStyle& style = ImGui::GetStyle();
    const float layoutLabelWidth = ImGui::CalcTextSize(kLayoutPresetLabel).x;
    const float opacityLabelWidth = ImGui::CalcTextSize(kOpacityLabel).x;
    const float layoutGroupWidth = (std::max)(layoutLabelWidth, kLayoutComboWidth * metrics.scale);
    const float opacityGroupWidth = (std::max)(opacityLabelWidth, kOpacitySliderWidth * metrics.scale);
    return layoutGroupWidth + style.ItemSpacing.x + opacityGroupWidth;
}

float MeasureControlsRowHeight(const LayoutMetrics&)
{
    return ImGui::GetTextLineHeight() + ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeight();
}

float MeasureContentWidth(const LayoutMetrics& metrics, bool showDebugPanel)
{
    const float headerWidth = MeasureHeaderRowWidth(metrics);
    const float controlsWidth = MeasureControlsRowWidth(metrics);
    const float clusterWidth = MeasureMaxRowWidth(metrics) + metrics.rowPaddingX * 2.0f + (kKeyStatesSectionInset * metrics.scale);
    const float footerWidth = ImGui::CalcTextSize(kFuturePlanText).x;

    float contentWidth = (std::max)(headerWidth, (std::max)(controlsWidth, (std::max)(clusterWidth, footerWidth)));
    if (showDebugPanel)
    {
        const float debugWidth = ImGui::CalcTextSize(kDebugHintText).x;
        contentWidth = (std::max)(contentWidth, debugWidth);
    }

    return contentWidth;
}

ImVec2 ComputePreferredWindowSize(const LayoutMetrics& metrics, bool showDebugPanel)
{
    const ImGuiStyle& style = ImGui::GetStyle();
    const float contentWidth = MeasureContentWidth(metrics, showDebugPanel);
    const float headerRowHeight = (std::max)(metrics.dragBarHeight, ImGui::GetTextLineHeight());
    const float controlsRowHeight = MeasureControlsRowHeight(metrics);
    const float clusterHeight = MeasureClusterBackgroundHeight(metrics);
    const float keyboardFooterHeight = ImGui::GetTextLineHeightWithSpacing();
    const float debugHeight = showDebugPanel ? (ImGui::GetTextLineHeightWithSpacing() * 2.0f) : 0.0f;

    float windowWidth = contentWidth + style.WindowPadding.x * 2.0f + 24.0f * metrics.scale;
    float windowHeight =
        style.WindowPadding.y * 2.0f +
        headerRowHeight +
        style.ItemSpacing.y +
        controlsRowHeight +
        style.ItemSpacing.y +
        style.ItemSpacing.y + 1.0f * metrics.scale +
        ImGui::GetTextLineHeight() +
        kSectionGap * metrics.scale +
        clusterHeight +
        style.ItemSpacing.y +
        keyboardFooterHeight +
        debugHeight +
        24.0f * metrics.scale;

    windowWidth = (std::max)(windowWidth, kMinimumWindowWidth);
    windowHeight = (std::max)(windowHeight, kMinimumWindowHeight);
    return ImVec2(windowWidth, windowHeight);
}

void DrawKeyCap(const char* label, float animationValue, bool wasPressed, float keyWidth, const LayoutMetrics& metrics)
{
    ImGui::PushID(label);
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 8.0f * metrics.scale);
    ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 1.0f * metrics.scale);

    const ImVec4 baseColor = ImVec4(0.16f, 0.16f, 0.18f, 1.0f);
    const ImVec4 activeColor = ImVec4(0.20f, 0.58f, 0.96f, 1.0f);
    const ImVec4 flashColor = ImVec4(0.96f, 0.74f, 0.24f, 1.0f);
    const ImVec4 borderColor = ImVec4(0.46f, 0.74f, 1.00f, 0.30f + 0.34f * animationValue);

    ImVec4 buttonColor = LerpColor(baseColor, activeColor, animationValue);
    if (wasPressed)
    {
        buttonColor = LerpColor(buttonColor, flashColor, 0.35f);
    }

    ImGui::PushStyleColor(ImGuiCol_Button, buttonColor);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, buttonColor);
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, buttonColor);
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
    ImGui::PushStyleColor(ImGuiCol_Border, borderColor);

    const ImVec2 buttonSize = ImVec2(keyWidth, GetKeyVisualHeight(metrics));
    ImGui::Button(label, buttonSize);

    ImGui::PopStyleColor(5);
    ImGui::PopStyleVar(2);
    ImGui::PopID();
}

void ApplyLayoutPreset(int presetIndex, bool& showDebugPanel, float& layoutScale)
{
    // 这里仅调整已有参数，避免预设切换时出现无效果反馈。
    switch (presetIndex)
    {
    case 0:
        showDebugPanel = false;
        layoutScale = 0.85f;
        break;
    case 1:
        showDebugPanel = false;
        layoutScale = 1.0f;
        break;
    case 2:
        showDebugPanel = true;
        layoutScale = 1.0f;
        break;
    default:
        break;
    }
}
} // namespace

void OverlayUI::Initialize()
{
    m_keyGlowEffects.clear();
    m_dragInteractionActive = false;
}

void OverlayUI::Shutdown()
{
    m_keyGlowEffects.clear();
}

void OverlayUI::Update(float deltaSeconds, const InputService& inputService)
{
    for (const KeyRow& row : kTrackedRows)
    {
        for (std::size_t i = 0; i < row.keyCount; ++i)
        {
            const KeyBinding& binding = row.keys[i];
            GlowEffect& glow = m_keyGlowEffects[binding.keyCode];
            const bool isDown = inputService.IsKeyDown(binding.keyCode);
            glow.SetTargetIntensity(isDown ? 1.0f : 0.0f);
            glow.Update(deltaSeconds);
        }
    }
}

ImVec2 OverlayUI::GetPreferredWindowSize() const
{
    return ComputePreferredWindowSize(BuildLayoutMetrics(m_layoutScale), m_showDebugPanel);
}

float OverlayUI::GetOverlayOpacity() const
{
    return m_overlayOpacity;
}

void OverlayUI::Render(const InputService& inputService)
{
    const LayoutMetrics metrics = BuildLayoutMetrics(m_layoutScale);
    const ImVec2 preferredSize = ComputePreferredWindowSize(metrics, m_showDebugPanel);

    const ImGuiWindowFlags windowFlags =
        ImGuiWindowFlags_NoDecoration |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoSavedSettings |
        ImGuiWindowFlags_NoFocusOnAppearing |
        ImGuiWindowFlags_NoNav |
        ImGuiWindowFlags_NoResize;

    ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f), ImGuiCond_Always);
    ImGui::SetNextWindowSize(preferredSize, ImGuiCond_Always);
    ImGui::SetNextWindowBgAlpha(m_overlayOpacity);

    const ImGuiStyle& style = ImGui::GetStyle();
    const float windowBorderSize = m_overlayOpacity <= 0.0f ? 0.0f : style.WindowBorderSize;
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, windowBorderSize);
    ImGui::Begin(kOverlayTitle, nullptr, windowFlags);
    ImGui::PopStyleVar();
    ImGui::PushStyleVar(ImGuiStyleVar_Alpha, m_overlayOpacity);

    ImGui::TextUnformatted(kOverlayTitle);
    ImGui::SameLine();

    const float dragButtonWidth = MeasureButtonWidth(kDragButtonLabel, metrics);
    ImGui::Button(kDragButtonLabel, ImVec2(dragButtonWidth, metrics.dragBarHeight));
    const bool dragPressed = ImGui::IsItemActive() && ImGui::IsMouseDown(ImGuiMouseButton_Left);
    if (dragPressed != m_dragInteractionActive)
    {
        m_dragInteractionActive = dragPressed;
        if (m_dragStateRequestHandler != nullptr)
        {
            m_dragStateRequestHandler(m_dragStateRequestContext, m_dragInteractionActive);
        }
    }

    if (m_dragInteractionActive && m_dragRequestHandler != nullptr && ImGui::IsMouseDragging(ImGuiMouseButton_Left))
    {
        const ImVec2 mouseDelta = ImGui::GetIO().MouseDelta;
        if (mouseDelta.x != 0.0f || mouseDelta.y != 0.0f)
        {
            m_dragRequestHandler(m_dragRequestContext, static_cast<int>(mouseDelta.x), static_cast<int>(mouseDelta.y));
        }
        ImGui::SetMouseCursor(ImGuiMouseCursor_ResizeAll);
    }

    ImGui::SameLine();
    const float exitButtonWidth = MeasureButtonWidth(kExitButtonLabel, metrics);
    const float exitButtonX = ImGui::GetCursorPosX() + ImGui::GetContentRegionAvail().x - exitButtonWidth;
    ImGui::SetCursorPosX(exitButtonX > ImGui::GetCursorPosX() ? exitButtonX : ImGui::GetCursorPosX());
    if (ImGui::Button(kExitButtonLabel, ImVec2(exitButtonWidth, 0.0f)))
    {
        if (m_exitRequestHandler != nullptr)
        {
            m_exitRequestHandler(m_exitRequestContext);
        }
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::BeginGroup();
    ImGui::TextUnformatted(kLayoutPresetLabel);
    ImGui::SetNextItemWidth(kLayoutComboWidth * metrics.scale);
    if (ImGui::Combo("##LayoutPreset", &m_layoutPresetIndex, kLayoutPresetLabels, static_cast<int>(kLayoutPresetCount)))
    {
        ApplyLayoutPreset(m_layoutPresetIndex, m_showDebugPanel, m_layoutScale);
    }
    ImGui::EndGroup();

    ImGui::SameLine();
    ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 1.0f);
    ImGui::BeginGroup();
    ImGui::TextUnformatted(kOpacityLabel);
    ImGui::SetNextItemWidth(kOpacitySliderWidth * metrics.scale);
    int opacityPercent = static_cast<int>(m_overlayOpacity * 100.0f + 0.5f);
    if (ImGui::SliderInt("##OverlayOpacity", &opacityPercent, 0, 100, "%d%%"))
    {
        m_overlayOpacity = static_cast<float>(opacityPercent) / 100.0f;
    }
    ImGui::EndGroup();
    ImGui::PopStyleVar();

    ImGui::Spacing();
    ImGui::Separator();
    DrawKeyboardVisualizer(inputService);
    if (m_showDebugPanel)
    {
        ImGui::Separator();
        ImGui::TextUnformatted(kDebugHintText);
        ImGui::TextDisabled(kDebugTodoText);
    }

    ImGui::PopStyleVar();
    ImGui::End();
}

void OverlayUI::DrawKeyboardVisualizer(const InputService& inputService)
{
    const LayoutMetrics metrics = BuildLayoutMetrics(m_layoutScale);
    const float sectionInset = kKeyStatesSectionInset * metrics.scale;

    ImGui::Indent(sectionInset);
    ImGui::TextUnformatted(kKeyStatesLabel);
    ImGui::Dummy(ImVec2(0.0f, kSectionGap * metrics.scale));

    std::array<float, kTrackedRows.size()> rowWidths{};
    float clusterWidthMax = 0.0f;
    for (std::size_t rowIndex = 0; rowIndex < kTrackedRows.size(); ++rowIndex)
    {
        rowWidths[rowIndex] = MeasureRowWidth(kTrackedRows[rowIndex].keys, kTrackedRows[rowIndex].keyCount, metrics);
        clusterWidthMax = (std::max)(clusterWidthMax, rowWidths[rowIndex]);
    }

    const float clusterContentHeight = MeasureClusterContentHeight(metrics);

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
            const auto glowIt = m_keyGlowEffects.find(binding.keyCode);
            if (glowIt != m_keyGlowEffects.end())
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
    for (std::size_t rowIndex = 0; rowIndex < kTrackedRows.size(); ++rowIndex)
    {
        drawRow(
            kTrackedRows[rowIndex].keys,
            kTrackedRows[rowIndex].keyCount,
            static_cast<float>(rowIndex) * rowStepY,
            rowWidths[rowIndex]);
    }

    ImGui::Dummy(ImVec2(clusterWidthMax, clusterContentHeight));
    ImGui::Spacing();
    ImGui::TextDisabled(kFuturePlanText);
    ImGui::Unindent(sectionInset);
}

void OverlayUI::SetShowDebugPanel(bool show)
{
    m_showDebugPanel = show;
}

void OverlayUI::SetExitRequestHandler(ExitRequestHandler handler, void* context)
{
    m_exitRequestHandler = handler;
    m_exitRequestContext = context;
}

void OverlayUI::SetDragRequestHandler(DragRequestHandler handler, void* context)
{
    m_dragRequestHandler = handler;
    m_dragRequestContext = context;
}

void OverlayUI::SetDragStateRequestHandler(DragStateRequestHandler handler, void* context)
{
    m_dragStateRequestHandler = handler;
    m_dragStateRequestContext = context;
}

void OverlayUI::SetLayoutScale(float scale)
{
    m_layoutScale = std::clamp(scale, kMinLayoutScale, kMaxLayoutScale);
}
}
