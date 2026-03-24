#pragma once

#include <cstdint>
#include <unordered_map>

#include "KeyboardLayoutPresets.h"
#include "OverlayUILayout.h"
#include "effects/GlowEffect.h"
#include "input/InputService.h"

namespace keyviz
{
void DrawKeyboardCluster(
    const InputService& inputService,
    KeyRowSet rowSet,
    const LayoutMetrics& metrics,
    const std::unordered_map<std::uint32_t, GlowEffect>& keyGlowEffects,
    float sectionInset,
    float sectionGap,
    const char* sectionLabel);
} // namespace keyviz
