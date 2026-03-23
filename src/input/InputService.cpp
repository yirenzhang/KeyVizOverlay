#include "InputService.h"

namespace keyviz
{
namespace
{
void ApplyKeyState(std::unordered_map<std::uint32_t, KeyState>& keyStates, std::uint32_t keyCode, bool isDown, bool pressedThisFrame, bool releasedThisFrame)
{
    KeyState& state = keyStates[keyCode];
    state.isDown = isDown;
    state.pressedThisFrame = pressedThisFrame;
    state.releasedThisFrame = releasedThisFrame;
    if (isDown)
    {
        state.downTimeSeconds = 0.0f;
    }
}

void ApplyModifierState(std::unordered_map<std::uint32_t, KeyState>& keyStates, std::uint32_t keyCode, bool isDown, bool pressedThisFrame, bool releasedThisFrame)
{
    if (keyCode == VK_SHIFT || keyCode == VK_LSHIFT || keyCode == VK_RSHIFT)
    {
        ApplyKeyState(keyStates, VK_SHIFT, isDown, pressedThisFrame, releasedThisFrame);
        ApplyKeyState(keyStates, VK_LSHIFT, isDown, pressedThisFrame, releasedThisFrame);
        ApplyKeyState(keyStates, VK_RSHIFT, isDown, pressedThisFrame, releasedThisFrame);
        return;
    }

    if (keyCode == VK_CONTROL || keyCode == VK_LCONTROL || keyCode == VK_RCONTROL)
    {
        ApplyKeyState(keyStates, VK_CONTROL, isDown, pressedThisFrame, releasedThisFrame);
        ApplyKeyState(keyStates, VK_LCONTROL, isDown, pressedThisFrame, releasedThisFrame);
        ApplyKeyState(keyStates, VK_RCONTROL, isDown, pressedThisFrame, releasedThisFrame);
        return;
    }

    if (keyCode == VK_MENU || keyCode == VK_LMENU || keyCode == VK_RMENU)
    {
        ApplyKeyState(keyStates, VK_MENU, isDown, pressedThisFrame, releasedThisFrame);
        ApplyKeyState(keyStates, VK_LMENU, isDown, pressedThisFrame, releasedThisFrame);
        ApplyKeyState(keyStates, VK_RMENU, isDown, pressedThisFrame, releasedThisFrame);
        return;
    }

    ApplyKeyState(keyStates, keyCode, isDown, pressedThisFrame, releasedThisFrame);
}
}

void InputService::Initialize()
{
    m_keyStates.clear();
    m_recentEvents.clear();
    m_rawInputBuffer.clear();
}

void InputService::Shutdown()
{
    m_keyStates.clear();
    m_recentEvents.clear();
    m_rawInputBuffer.clear();
}

void InputService::Update(float deltaSeconds)
{
    if (deltaSeconds < 0.0f)
    {
        deltaSeconds = 0.0f;
    }

    const float riseSpeed = 12.0f;
    const float fallSpeed = 10.0f;

    for (auto& [keyCode, state] : m_keyStates)
    {
        (void)keyCode;

        const float target = state.isDown ? 1.0f : 0.0f;
        const float speed = state.isDown ? riseSpeed : fallSpeed;
        const float step = speed * deltaSeconds;

        if (state.animationProgress < target)
        {
            state.animationProgress = (state.animationProgress + step < target) ? (state.animationProgress + step) : target;
        }
        else if (state.animationProgress > target)
        {
            state.animationProgress = (state.animationProgress - step > target) ? (state.animationProgress - step) : target;
        }

        if (state.isDown)
        {
            state.downTimeSeconds += deltaSeconds;
        }
        else
        {
            state.downTimeSeconds = 0.0f;
        }
    }
}

void InputService::HandleEvent(const InputEvent& event)
{
    m_recentEvents.push_back(event);

    const bool isDown = event.type == InputEvent::Type::KeyDown;
    const bool pressedThisFrame = isDown;
    const bool releasedThisFrame = event.type == InputEvent::Type::KeyUp;
    ApplyModifierState(m_keyStates, event.keyCode, isDown, pressedThisFrame, releasedThisFrame);
}

void InputService::HandleRawInput(HRAWINPUT rawInput)
{
    UINT requiredSize = 0;
    if (GetRawInputData(rawInput, RID_INPUT, nullptr, &requiredSize, sizeof(RAWINPUTHEADER)) != 0 || requiredSize == 0)
    {
        return;
    }

    m_rawInputBuffer.resize(requiredSize);
    if (GetRawInputData(rawInput, RID_INPUT, m_rawInputBuffer.data(), &requiredSize, sizeof(RAWINPUTHEADER)) != requiredSize)
    {
        return;
    }

    const RAWINPUT* raw = reinterpret_cast<const RAWINPUT*>(m_rawInputBuffer.data());
    if (raw->header.dwType == RIM_TYPEKEYBOARD)
    {
        const RAWKEYBOARD& keyboard = raw->data.keyboard;
        if (keyboard.VKey == 0U || keyboard.VKey == 255U)
        {
            return;
        }

        InputEvent event{};
        event.keyCode = keyboard.VKey;
        event.timestampMs = static_cast<std::uint64_t>(GetMessageTime());
        event.type = (keyboard.Flags & RI_KEY_BREAK) ? InputEvent::Type::KeyUp : InputEvent::Type::KeyDown;
        event.isRepeat = false;
        HandleEvent(event);
        return;
    }

    if (raw->header.dwType != RIM_TYPEMOUSE)
    {
        return;
    }

    const RAWMOUSE& mouse = raw->data.mouse;
    const USHORT buttonFlags = mouse.usButtonFlags;
    const std::uint64_t timestampMs = static_cast<std::uint64_t>(GetMessageTime());

    auto emitMouseButtonEvent = [&](std::uint32_t keyCode, bool pressed) {
        InputEvent event{};
        event.keyCode = keyCode;
        event.timestampMs = timestampMs;
        event.type = pressed ? InputEvent::Type::KeyDown : InputEvent::Type::KeyUp;
        event.isRepeat = false;
        HandleEvent(event);
    };

    if ((buttonFlags & RI_MOUSE_LEFT_BUTTON_DOWN) != 0)
    {
        emitMouseButtonEvent(VK_LBUTTON, true);
    }
    if ((buttonFlags & RI_MOUSE_LEFT_BUTTON_UP) != 0)
    {
        emitMouseButtonEvent(VK_LBUTTON, false);
    }
    if ((buttonFlags & RI_MOUSE_RIGHT_BUTTON_DOWN) != 0)
    {
        emitMouseButtonEvent(VK_RBUTTON, true);
    }
    if ((buttonFlags & RI_MOUSE_RIGHT_BUTTON_UP) != 0)
    {
        emitMouseButtonEvent(VK_RBUTTON, false);
    }
    if ((buttonFlags & RI_MOUSE_MIDDLE_BUTTON_DOWN) != 0)
    {
        emitMouseButtonEvent(VK_MBUTTON, true);
    }
    if ((buttonFlags & RI_MOUSE_MIDDLE_BUTTON_UP) != 0)
    {
        emitMouseButtonEvent(VK_MBUTTON, false);
    }
    if ((buttonFlags & RI_MOUSE_BUTTON_4_DOWN) != 0)
    {
        emitMouseButtonEvent(VK_XBUTTON1, true);
    }
    if ((buttonFlags & RI_MOUSE_BUTTON_4_UP) != 0)
    {
        emitMouseButtonEvent(VK_XBUTTON1, false);
    }
    if ((buttonFlags & RI_MOUSE_BUTTON_5_DOWN) != 0)
    {
        emitMouseButtonEvent(VK_XBUTTON2, true);
    }
    if ((buttonFlags & RI_MOUSE_BUTTON_5_UP) != 0)
    {
        emitMouseButtonEvent(VK_XBUTTON2, false);
    }
}

const KeyState* InputService::TryGetKeyState(std::uint32_t keyCode) const
{
    const auto it = m_keyStates.find(keyCode);
    if (it == m_keyStates.end())
    {
        return nullptr;
    }

    return &it->second;
}

bool InputService::IsKeyDown(std::uint32_t keyCode) const
{
    const KeyState* state = TryGetKeyState(keyCode);
    return state != nullptr && state->isDown;
}

bool InputService::WasPressedThisFrame(std::uint32_t keyCode) const
{
    const KeyState* state = TryGetKeyState(keyCode);
    return state != nullptr && state->pressedThisFrame;
}

bool InputService::WasReleasedThisFrame(std::uint32_t keyCode) const
{
    const KeyState* state = TryGetKeyState(keyCode);
    return state != nullptr && state->releasedThisFrame;
}

void InputService::ClearFrameFlags()
{
    for (auto& [keyCode, state] : m_keyStates)
    {
        (void)keyCode;
        state.pressedThisFrame = false;
        state.releasedThisFrame = false;
    }

    m_recentEvents.clear();
}
}
