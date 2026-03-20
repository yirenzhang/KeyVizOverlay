#include "OverlayUI.h"

#include <cstddef>

namespace keyviz
{
namespace
{
    // TODO: 闂傚倸鍊风粈渚€骞夐敓鐘冲殞闁诡垼鐏愯ぐ鎺撳€婚柤鎭掑劚娴犺櫣绱撻崒娆戝妽閽冨崬霉濠у灝鈧牜鎹㈠☉銏犲耿婵☆垵顕х喊宥呪攽閻愬弶鍣抽柍褜鍓濈粔顔裤亹閹烘垹顦ч梺鍏肩ゴ閺呪晠寮抽埄鍐瘈闁汇垽娼ф禒婊堟煥閺囶亞鐣垫鐐村姍楠炴帒螖閳ь剟鎮炲ú顏呯厱闁规壋鏅涙俊鍨瑰鎰М闁哄矉缍佹慨鈧柍杞拌兌娴犫晠姊洪崫銉ユ珡闁稿锕獮鍡涘炊椤掑鏅㈤梺鍛婃处閸嬪懘藟濠靛鐓熼柣鏂挎憸閹冲啴鎮楀鍗炲幋鐎殿喓鍔戦獮搴ㄦ嚍閵夈儮鍋撻悽鍛婄厽闁靛繆妲呴崯蹇涙煛閳ь剚瀵肩€涙鍙冮梺鍝勮癁閸愬彞绱旈梻浣芥〃缁€浣虹矓閼哥數顩烽柨鏇炲€归崐閿嬨亜閹哄秶鍔嶆い鏂挎喘濮婄粯鎷呴崫銉ㄩ梺鍝勬嚀鐏忔瑥鈻庨姀鐙€娼╅悹铏瑰劋閻庮剙顪冮妶鍡樺蔼闁搞劍妞藉畷鏇熸償椤厾绠氬銈嗙墬绾板秹骞嗛崼鈶╁亾濞堝灝鏋涙繛灏栤偓鎰佹綎闁告繂瀚ч弸搴ㄦ煙椤栧棗瀚禍顏堟⒑鐠囪尙绠版繛瀛樺哺瀹曞綊鎳滈崗鍝ョ畾闁诲孩绋掕摫濠殿垱鎸抽弻娑樷攽閸曨偄濮曢梺鍦厴娴滆泛顫忕紒妯肩懝闁搞儜鍕簴闂備礁鎲￠弻銊╂儔閸忚偐顩查柟闂寸贰閺佸秹鏌ｉ幇顒€绾ч柕鍥ㄧ箞閹宕楁径濠佸闂備礁鎲″ú锕傚储妤ｅ啫纾归柣鎰劋閳锋垿鏌涘┑鍡楊伀鐎涙繂鈹戦悙棰濆殝濠碘€虫喘閸┾偓妞ゆ巻鍋撻柣蹇旀皑閸掓帡骞樼拠鑼舵憰闂侀潧艌閺呪晠寮繝鍥ㄧ厽闁规崘娅曢崬澶嬨亜閺傛妯€闁哄矉缍侀幃銏ゅ传閵夛箑娅戞繝纰樻閸嬪懘銆冮崼銉ョ劦妞ゆ巻鍋撻柣蹇斿哺閹繝鍨惧畷鍥ㄦ濠殿喗顭堝▔娑㈡倷婵犲洦鐓忓┑鐐茬仢閸旀氨绱掗妸锝呭姦婵?
struct KeyBinding
{
    const char* label;
    std::uint32_t keyCode;
    float widthScale;
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
    { "ESC", VK_ESCAPE, 0.82f },
    { "W", 'W', 1.00f },
};

constexpr KeyBinding kMiddleRowKeys[] =
{
    { "A", 'A', 1.00f },
    { "S", 'S', 1.00f },
    { "D", 'D', 1.00f },
};

constexpr KeyBinding kBottomRowKeys[] =
{
    { "Shift", VK_SHIFT, 1.15f },
    { "Space", VK_SPACE, 2.60f },
    { "Ctrl", VK_CONTROL, 1.05f },
    { "Alt", VK_MENU, 1.05f },
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
constexpr std::size_t kLayoutPresetCount = sizeof(kLayoutPresetLabels) / sizeof(kLayoutPresetLabels[0]);
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
    if (scale < kMinLayoutScale)
    {
        scale = kMinLayoutScale;
    }
    else if (scale > kMaxLayoutScale)
    {
        scale = kMaxLayoutScale;
    }

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
    if (t < 0.0f)
    {
        t = 0.0f;
    }
    else if (t > 1.0f)
    {
        t = 1.0f;
    }

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
    return animationValue > glowValue ? animationValue : glowValue;
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
    const float topRowWidth = MeasureRowWidth(kTopRowKeys, kTopRowKeyCount, metrics);
    const float middleRowWidth = MeasureRowWidth(kMiddleRowKeys, kMiddleRowKeyCount, metrics);
    const float bottomRowWidth = MeasureRowWidth(kBottomRowKeys, kBottomRowKeyCount, metrics);
    float maxWidth = topRowWidth > middleRowWidth ? topRowWidth : middleRowWidth;
    maxWidth = maxWidth > bottomRowWidth ? maxWidth : bottomRowWidth;
    return maxWidth;
}

float MeasureClusterHeight(const LayoutMetrics& metrics)
{
    return (GetKeyVisualHeight(metrics) * 3.0f) + (metrics.rowSpacing * 2.0f) + (metrics.rowPaddingY * 2.0f);
}

float MeasureButtonWidth(const char* label, const LayoutMetrics& metrics)
{
    const ImGuiStyle& style = ImGui::GetStyle();
    return ImGui::CalcTextSize(label).x + style.FramePadding.x * 2.0f + 18.0f * metrics.scale;
}

float MeasureHeaderRowWidth(const LayoutMetrics& metrics)
{
    const ImGuiStyle& style = ImGui::GetStyle();
    const float titleWidth = ImGui::CalcTextSize("KeyViz Overlay").x;
    const float dragButtonWidth = MeasureButtonWidth("Drag to move", metrics);
    const float exitButtonWidth = MeasureButtonWidth("Exit", metrics);
    return titleWidth + style.ItemSpacing.x + dragButtonWidth + style.ItemSpacing.x + exitButtonWidth;
}

float MeasureControlsRowWidth(const LayoutMetrics& metrics)
{
    const ImGuiStyle& style = ImGui::GetStyle();
    const float layoutLabelWidth = ImGui::CalcTextSize("Layout preset").x;
    const float opacityLabelWidth = ImGui::CalcTextSize("Opacity").x;
    const float layoutGroupWidth = layoutLabelWidth > (kLayoutComboWidth * metrics.scale) ? layoutLabelWidth : (kLayoutComboWidth * metrics.scale);
    const float opacityGroupWidth = opacityLabelWidth > (kOpacitySliderWidth * metrics.scale) ? opacityLabelWidth : (kOpacitySliderWidth * metrics.scale);
    return layoutGroupWidth + style.ItemSpacing.x + opacityGroupWidth;
}

float MeasureControlsRowHeight(const LayoutMetrics& metrics)
{
    return ImGui::GetTextLineHeight() + ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeight();
}

float MeasureContentWidth(const LayoutMetrics& metrics, bool showDebugPanel)
{
    const float headerWidth = MeasureHeaderRowWidth(metrics);
    const float controlsWidth = MeasureControlsRowWidth(metrics);
    const float clusterWidth = MeasureMaxRowWidth(metrics) + metrics.rowPaddingX * 2.0f + (kKeyStatesSectionInset * metrics.scale);
    const float footerWidth = ImGui::CalcTextSize("TODO: Full keyboard layout and richer effect layers will be added later.").x;
    float contentWidth = headerWidth;
    contentWidth = contentWidth > controlsWidth ? contentWidth : controlsWidth;
    contentWidth = contentWidth > clusterWidth ? contentWidth : clusterWidth;
    contentWidth = contentWidth > footerWidth ? contentWidth : footerWidth;

    if (showDebugPanel)
    {
        const float debugWidth = ImGui::CalcTextSize("Hold a tracked key to animate the glow.").x;
        contentWidth = contentWidth > debugWidth ? contentWidth : debugWidth;
    }

    return contentWidth;
}

ImVec2 ComputePreferredWindowSize(const LayoutMetrics& metrics, bool showDebugPanel)
{
    const ImGuiStyle& style = ImGui::GetStyle();
    const float contentWidth = MeasureContentWidth(metrics, showDebugPanel);
    const float headerRowHeight = metrics.dragBarHeight > ImGui::GetTextLineHeight() ? metrics.dragBarHeight : ImGui::GetTextLineHeight();
    const float controlsRowHeight = MeasureControlsRowHeight(metrics);
    const float clusterHeight = MeasureClusterHeight(metrics);
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

    if (windowWidth < kMinimumWindowWidth)
    {
        windowWidth = kMinimumWindowWidth;
    }

    if (windowHeight < kMinimumWindowHeight)
    {
        windowHeight = kMinimumWindowHeight;
    }

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
} // namespace
void OverlayUI::Initialize()
{
    m_keyGlowEffects.clear();
    m_dragInteractionActive = false;
    // TODO: 闂傚倸鍊搁崐鎼佸磹妞嬪海鐭嗗〒姘ｅ亾妤犵偛顦甸弫鎾绘偐閸愬弶鐤勯梻浣筋嚃閸ㄥジ鎮橀幇顖樹汗闁圭儤鎸搁埀顒€顭烽弻銈夊箒閹烘垵濮庢繛瀵稿Х閸庛倗鎹㈠┑瀣仺闂傚牊鍒€閿濆鐓犵憸鐗堝笧閻ｇ儤顨ラ悙鏉戠伌鐎规洖銈稿鎾偄閸濆嫬濡囨繝鐢靛Х閺佹悂宕戝☉銏″€舵繝闈涙矗缁诲棝鎮归幁鎺戝缁炬儳銈搁弻锝呂熼崹顔炬缂備礁顦遍崕銈夊箞閵婏妇绡€闁告劏鏂傛禒銏犖旈悩闈涗沪闁圭懓娲悰顕€骞掑Δ鈧崡鎶芥煟閹存繃顥欑紒杈ㄦ濮婂宕掑▎鎴犵崲濠电偠澹堝畷鐢垫閻愬搫骞㈡繛鎴灻悗顓㈡⒑閸涘娈橀柛搴㈠姍瀹?UI 闂傚倸鍊搁崐鎼佸磹妞嬪海鐭嗗ù锝夋交閼板潡姊洪鈧粔鏌ュ焵椤掆偓閸婂湱绮嬮幒鏂哄亾閿濆簼绨介柨娑欑洴濮婃椽鎮烽弶搴撴寖缂備緡鍣崹鍫曞春濞戙垹绠虫俊銈勮兌閸橀亶姊洪棃娑辨▓闁搞劍濞婂畷姘跺级濡數鎳撻…銊╁醇閵忋垺姣囨繝娈垮枛閿曘倝骞栭锝嗘珡闂備焦鎮堕崕婊堝礃閸撗€鍋撻幎鑺モ拺閻犲洦褰冮銏ゆ煕閹存繄绉虹€规洘绮撻幃銏ゆ偂鎼达絿鏆┑掳鍊ч梽鍕敄濞嗗精锝嗙附閸涘ň鎷洪梺鑽ゅ枑濠㈡ê危婵犳碍鐓欓柧蹇ｅ亾閼板潡鏌ｅ☉鍗炴灈妞ゎ偅绮撻崺鈧い鎺戝瀹撲胶鈧箍鍎卞Λ娆愬垔閹绢喗鈷掗柍褜鍓熼獮鍥敆婢跺妫ユ繝鐢靛Х閺佹悂宕戦悩璇茬妞ゅ繐妫楃欢銈夋煢濡警妾ч柤鏉挎健瀵爼宕煎☉妯侯瀷闂佺粯绻嗗▔娑⑩€︾捄銊﹀磯闁告繂瀚峰Σ鐢告⒑缁嬫鍎愰柟鍛婃倐閸┿垽骞樼拠鎻掔€銈嗘⒒閺咁偅绂嶉鍡欑＝闁稿本鐟ㄩ崗宀€绱掗鍛仸鐎规洘绻傞埢搴ㄥ箻鐠鸿櫣銈﹂梻濠庡亜濞诧箓寮婚妸鈺婃晪闂侇剙绉甸崐鍨箾閹寸儐浠炬い蹇撴噽椤╄尙鎲搁悧鍫濈瑲闁抽攱鍨块弻锝夊箻閸愬樊鍔夐梺瑙勫絻閹诧繝濡甸崟顖涙櫜闁告洦鍘鹃悡鎾绘倵鐟欏嫭绀冮柛銊ユ健閻涱喖螣鐏忔牕浜炬繛鎴烆仾椤忓牜鏁侀柟鎯у绾惧ジ鏌涢幘妤€鍠氶弳顓㈡⒑閹稿氦澹橀棁?
}

void OverlayUI::Shutdown()
{
    m_keyGlowEffects.clear();
    // TODO: 闂傚倸鍊搁崐鎼佸磹妞嬪海鐭嗗〒姘ｅ亾妤犵偛顦甸弫鎾绘偐閸愬弶鐤勯梻浣筋嚃閸ㄥジ鎮橀幇顖樹汗闁圭儤鎸搁埀顒€顭烽弻銈夊箒閹烘垵濮庢繛瀵稿Х閸庛倗鎹㈠┑瀣仺闂傚牊鍒€閿濆鐓犵憸鐗堝笧閻ｇ儤顨ラ悙鏉戠伌鐎规洖銈稿鎾偄閸濆嫬濡囨繝鐢靛Х閺佹悂宕戝☉銏″€舵繝闈涙矗缁诲棝鎮归幁鎺戝缁炬儳銈搁弻锝呂熼崹顔炬缂備礁顦遍崕銈夊箞閵婏妇绡€闁告劏鏂傛禒銏犖旈悩闈涗沪闁圭懓娲悰顕€骞掑Δ鈧猾宥夋煕鐏炴崘澹樼悮鈺呮⒒閸屾艾鈧娆㈠顒夌劷闁煎鍊曠欢銈呂旈敐鍛殭缂佺姳鍗抽弻娑㈠箛椤掍讲鏋欓梺?UI 闂傚倸鍊搁崐宄懊归崶顒夋晪鐟滃繘骞戦姀銈呯婵°倐鍋撶痪鍓х帛缁绘盯骞嬪▎蹇曚患缂佺偓鍎崇紞濠囧蓟濞戙垺鏅滈悹鍥ㄥ絻缁犱即姊虹粙娆惧剱闁圭懓娲ら悾鐤亹閹烘繃鏅濋梺鎸庣箓鐎涒晠藟閸儲鐓熼幖娣焺閸熷繘鏌涢悩宕囧⒌闁诡喗锚椤繈顢楁径濠冾唫闂備礁鎲″ú锕傚储閸濆嫧鏌︽い蹇撶墛閻撴洟鏌熼悙顒夋當閻庢凹鍙冮、娆掔疀濞戞瑢鎷绘繛杈剧秬濡嫰宕ラ悷鎵虫斀妞ゆ棁鍋愭晥閻庤娲橀敃銏ゅ灳閿曞倸绠ｆ繝濠傛噺閿涘繒绱撻崒姘偓鎼佸磹妞嬪孩顐介柨鐔哄У閸嬪倿鏌涢鐘插姎闂傚偆鍨堕弻銊モ攽閸℃﹩妫涚紓?
}

void OverlayUI::Update(float deltaSeconds, const InputService& inputService)
{
    const KeyBinding* trackedRows[] =
    {
        kTopRowKeys,
        kMiddleRowKeys,
        kBottomRowKeys,
    };
    const std::size_t trackedRowCounts[] =
    {
        kTopRowKeyCount,
        kMiddleRowKeyCount,
        kBottomRowKeyCount,
    };

    for (std::size_t rowIndex = 0; rowIndex < 3; ++rowIndex)
    {
        for (std::size_t i = 0; i < trackedRowCounts[rowIndex]; ++i)
        {
            const KeyBinding& binding = trackedRows[rowIndex][i];
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
    ImGui::Begin("KeyViz Overlay", nullptr, windowFlags);
    ImGui::PushStyleVar(ImGuiStyleVar_Alpha, m_overlayOpacity);

    ImGui::TextUnformatted("KeyViz Overlay");
    ImGui::SameLine();

    const float dragButtonWidth = MeasureButtonWidth("Drag to move", metrics);
    ImGui::Button("Drag to move", ImVec2(dragButtonWidth, metrics.dragBarHeight));
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
    const float exitButtonWidth = MeasureButtonWidth("Exit", metrics);
    const float exitButtonX = ImGui::GetCursorPosX() + ImGui::GetContentRegionAvail().x - exitButtonWidth;
    ImGui::SetCursorPosX(exitButtonX > ImGui::GetCursorPosX() ? exitButtonX : ImGui::GetCursorPosX());
    if (ImGui::Button("Exit", ImVec2(exitButtonWidth, 0.0f)))
    {
        if (m_exitRequestHandler != nullptr)
        {
            m_exitRequestHandler(m_exitRequestContext);
        }
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::BeginGroup();
    ImGui::TextUnformatted("Layout preset");
    ImGui::SetNextItemWidth(kLayoutComboWidth * metrics.scale);
    if (ImGui::Combo("##LayoutPreset", &m_layoutPresetIndex, kLayoutPresetLabels, static_cast<int>(kLayoutPresetCount)))
    {
        // TODO: 闂傚倸鍊搁崐椋庣矆娓氣偓楠炲鏁撻悩鍐叉疄闂佽鍨奸悘鎰亹閹烘挸鈧鏌ら幁鎺戝姎濞寸姷鍎ょ换婵嬫偨闂堟刀锝夋煠閻熸澘鈷旂紒顔碱煼椤㈡瑧浜搁弽銊︽澑闂備胶绮…鍫㈠垝妤ｅ啫绀傜€光偓閸曨剛鍘介梺缁樻煥閹芥粎绮婚悙鐑樼厸閻忕偛澧介埥澶嬨亜椤愶絿绠炴い銏☆殕閹峰懐鎲撮崟顐紗闂傚倸鍊风粈渚€骞栭锔藉剶濠靛倻顭堢粣妤呭箹濞ｎ剙濡搁柍褜鍓欓幊蹇曠紦閻ｅ瞼鐭欓柛褎顨呴弫褰掓⒒娓氣偓濞佳呮崲閸℃稑鐒垫い鎺嗗亾缁剧虎鍙冨畷婵嬵敍濞戞氨鐦堝┑鐐茬墕閻忔繈寮搁悢铏圭＜缂備焦锚婵鈧灚婢橀敃顏勭暦濠婂棭妲奸梺鍝勬缁捇寮婚悢铏圭＜闁靛繒濮甸悘鍫㈢磽娴ｆ彃浜鹃梺绋跨灱閸嬬偤鎮¤箛鎿冪唵闁煎摜鏁搁妴鎺楁煟閿濆鎲鹃柡宀嬬秮閺佸啴鍩€椤掑嫧鈧箑鐣￠柇锔界稁闂佸憡绻傜€氀囧几鎼淬劍鐓欐い鏍ф鐎氼喗绂嶉鍡欑＝闁稿本鐟ч崝宥嗕繆椤愩垹鏆ｉ挊鐔兼煙閹屽殶妞も晝鍏橀弻銊╁即閻愭祴鍋撻幖浣瑰€块柛顭戝亖娴滄粓鏌熼崫鍕棞濞存粓绠栧娲濞戞瑯妫￠柣銏╁灙閳ь剙纾弳锕傛煥濠靛棙顥滅紒鍓佸仱閹﹢鎮欓懜娈挎闂佸吋鎯岄崹璺侯潖?
    }
    ImGui::EndGroup();

    ImGui::PopStyleVar();

    ImGui::SameLine();
    ImGui::BeginGroup();
    ImGui::TextUnformatted("Opacity");
    ImGui::SetNextItemWidth(kOpacitySliderWidth * metrics.scale);
    int opacityPercent = static_cast<int>(m_overlayOpacity * 100.0f + 0.5f);
    if (ImGui::SliderInt("##OverlayOpacity", &opacityPercent, 0, 100, "%d%%"))
    {
        m_overlayOpacity = static_cast<float>(opacityPercent) / 100.0f;
    }
    ImGui::EndGroup();

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::PushStyleVar(ImGuiStyleVar_Alpha, m_overlayOpacity);
    DrawKeyboardVisualizer(inputService);
    if (m_showDebugPanel)
    {
        ImGui::Separator();
        ImGui::TextUnformatted("Hold a tracked key to animate the glow.");
        ImGui::TextDisabled("TODO: Expand to the full keyboard layout later.");
    }

    ImGui::PopStyleVar();

    ImGui::End();
}

void OverlayUI::DrawKeyboardVisualizer(const InputService& inputService)
{
    const LayoutMetrics metrics = BuildLayoutMetrics(m_layoutScale);
    const float sectionInset = kKeyStatesSectionInset * metrics.scale;
    ImGui::Indent(sectionInset);
    ImGui::TextUnformatted("Key states");
    ImGui::Dummy(ImVec2(0.0f, kSectionGap * metrics.scale));

    const float topRowWidth = MeasureRowWidth(kTopRowKeys, kTopRowKeyCount, metrics);
    const float middleRowWidth = MeasureRowWidth(kMiddleRowKeys, kMiddleRowKeyCount, metrics);
    const float bottomRowWidth = MeasureRowWidth(kBottomRowKeys, kBottomRowKeyCount, metrics);
    const float clusterWidth = topRowWidth > middleRowWidth ? topRowWidth : middleRowWidth;
    const float clusterWidthMax = clusterWidth > bottomRowWidth ? clusterWidth : bottomRowWidth;
    const float clusterHeight = (GetKeyVisualHeight(metrics) * 3.0f) + (metrics.rowSpacing * 2.0f);

    const ImVec2 clusterOrigin = ImGui::GetCursorScreenPos();
    const ImVec2 clusterMin = ImVec2(clusterOrigin.x - metrics.rowPaddingX, clusterOrigin.y - metrics.rowPaddingY);
    const ImVec2 clusterMax = ImVec2(
        clusterOrigin.x + clusterWidthMax + metrics.rowPaddingX,
        clusterOrigin.y + clusterHeight + metrics.rowPaddingY);

    ImDrawList* drawList = ImGui::GetWindowDrawList();
    drawList->AddRectFilled(clusterMin, clusterMax, ImGui::GetColorU32(ImVec4(0.08f, 0.09f, 0.11f, 0.58f)), 16.0f * metrics.scale);
    drawList->AddRect(clusterMin, clusterMax, ImGui::GetColorU32(ImVec4(0.38f, 0.56f, 0.86f, 0.26f)), 16.0f * metrics.scale, 0, 1.0f * metrics.scale);

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

    drawRow(kTopRowKeys, kTopRowKeyCount, 0.0f, topRowWidth);
    drawRow(kMiddleRowKeys, kMiddleRowKeyCount, metrics.keyHeight + metrics.rowSpacing, middleRowWidth);
    drawRow(kBottomRowKeys, kBottomRowKeyCount, (metrics.keyHeight + metrics.rowSpacing) * 2.0f, bottomRowWidth);

    ImGui::Dummy(ImVec2(clusterWidthMax, clusterHeight));
    ImGui::Spacing();
    ImGui::TextDisabled("TODO: Full keyboard layout and richer effect layers will be added later.");
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
    m_layoutScale = scale;
}
}
