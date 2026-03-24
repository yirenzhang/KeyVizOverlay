#include "OverlayUIConfig.h"

namespace keyviz
{
const OverlayUIConfig& GetOverlayUIConfig()
{
    static const OverlayUIConfig config = []() {
        OverlayUIConfig value{};
        value.overlayTitle = "KeyViz Overlay";
        value.dragButtonLabel = "Drag to move";
        value.helpButtonLabel = "Help";
        value.aboutButtonLabel = "About";
        value.exitButtonLabel = "Exit";
        value.layoutPresetLabel = "Layout preset";
        value.opacityLabel = "Opacity";
        value.keySizeLabel = "Key size";
        value.keyStatesLabel = "Key states";
        value.sectionGap = 16.0f;
        value.keyStatesSectionInset = 16.0f;
        value.layoutComboWidth = 164.0f;
        value.opacitySliderWidth = 150.0f;
        value.keySizeSliderWidth = 150.0f;
        value.minimumWindowWidth = 420.0f;
        value.minimumWindowHeight = 240.0f;
        return value;
    }();
    return config;
}

OverlayPanelMetricsConfig BuildOverlayPanelMetricsConfig(const OverlayUIConfig& config)
{
    OverlayPanelMetricsConfig metricsConfig{};
    metricsConfig.title = config.overlayTitle;
    metricsConfig.dragButtonLabel = config.dragButtonLabel;
    metricsConfig.helpButtonLabel = config.helpButtonLabel;
    metricsConfig.aboutButtonLabel = config.aboutButtonLabel;
    metricsConfig.exitButtonLabel = config.exitButtonLabel;
    metricsConfig.layoutLabel = config.layoutPresetLabel;
    metricsConfig.opacityLabel = config.opacityLabel;
    metricsConfig.keySizeLabel = config.keySizeLabel;
    metricsConfig.keyStatesSectionInset = config.keyStatesSectionInset;
    metricsConfig.layoutComboWidth = config.layoutComboWidth;
    metricsConfig.opacitySliderWidth = config.opacitySliderWidth;
    metricsConfig.keySizeSliderWidth = config.keySizeSliderWidth;
    metricsConfig.minimumWindowWidth = config.minimumWindowWidth;
    metricsConfig.minimumWindowHeight = config.minimumWindowHeight;
    metricsConfig.sectionGap = config.sectionGap;
    return metricsConfig;
}

ImGuiWindowFlags GetOverlayWindowFlags()
{
    return ImGuiWindowFlags_NoDecoration |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoSavedSettings |
        ImGuiWindowFlags_NoFocusOnAppearing |
        ImGuiWindowFlags_NoNav |
        ImGuiWindowFlags_NoResize;
}
} // namespace keyviz
