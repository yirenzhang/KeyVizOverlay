#include "OverlayFontAtlas.h"

#include <array>

namespace keyviz
{
void BuildOverlayFontAtlas(ImGuiIO& io)
{
    // 仅键帽使用项目内像素字体；其余界面保持默认字体。
    static constexpr std::array<float, 6> kKeyFontSizes =
    {
        12.0f, 16.0f, 20.0f, 24.0f, 28.0f, 32.0f
    };
    constexpr const char* kPixelFontPath = "assets/fonts/PixeloidMono.ttf";

    io.Fonts->Clear();
    ImFont* defaultFont = io.Fonts->AddFontDefault();

    for (const float size : kKeyFontSizes)
    {
        ImFontConfig config{};
        config.SizePixels = size;
        config.PixelSnapH = true;
        config.OversampleH = 1;
        config.OversampleV = 1;

        if (io.Fonts->AddFontFromFileTTF(kPixelFontPath, size, &config) == nullptr)
        {
            // 像素字体缺失时回退默认字体，避免字体表为空导致渲染异常。
            io.Fonts->AddFontDefault(&config);
        }
    }

    if (defaultFont != nullptr)
    {
        io.FontDefault = defaultFont;
    }
}
} // namespace keyviz
