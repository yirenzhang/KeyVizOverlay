#pragma once

#include <cstdint>
#include <unordered_map>

#include "KeyboardLayoutPresets.h"
#include "effects/GlowEffect.h"
#include "input/InputService.h"

namespace keyviz
{
void UpdateKeyGlowStates(
    float deltaSeconds,
    const InputService& inputService,
    KeyRowSet rowSet,
    std::unordered_map<std::uint32_t, GlowEffect>& keyGlowEffects);
} // namespace keyviz
