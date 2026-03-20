#pragma once

#include <cstdint>
#include <vector>
#include <unordered_map>

#include <windows.h>

#include "models/InputEvent.h"
#include "models/KeyState.h"

namespace keyviz
{
class InputService
{
public:
    InputService() = default;

    void Initialize();
    void Shutdown();
    void Update(float deltaSeconds);
    void HandleEvent(const InputEvent& event);
    void HandleRawInput(HRAWINPUT rawInput);

    const KeyState* TryGetKeyState(std::uint32_t keyCode) const;
    bool IsKeyDown(std::uint32_t keyCode) const;
    bool WasPressedThisFrame(std::uint32_t keyCode) const;
    bool WasReleasedThisFrame(std::uint32_t keyCode) const;
    void ClearFrameFlags();

private:
    std::unordered_map<std::uint32_t, KeyState> m_keyStates;
    std::vector<InputEvent> m_recentEvents;
    std::vector<std::uint8_t> m_rawInputBuffer;
};
}
