#include "OverlayStateUpdater.h"

namespace keyviz
{
void UpdateKeyGlowStates(
    float deltaSeconds,
    const InputService& inputService,
    KeyRowSet rowSet,
    std::unordered_map<std::uint32_t, GlowEffect>& keyGlowEffects)
{
    for (std::size_t rowIndex = 0; rowIndex < rowSet.count; ++rowIndex)
    {
        const KeyRow& row = rowSet.rows[rowIndex];
        for (std::size_t i = 0; i < row.keyCount; ++i)
        {
            const KeyBinding& binding = row.keys[i];
            GlowEffect& glow = keyGlowEffects[binding.keyCode];
            const bool isDown = inputService.IsKeyDown(binding.keyCode);
            glow.SetTargetIntensity(isDown ? 1.0f : 0.0f);
            glow.Update(deltaSeconds);
        }
    }
}
} // namespace keyviz
