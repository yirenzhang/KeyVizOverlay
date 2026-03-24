#pragma once

#include <string>

namespace keyviz
{
class InputService;

enum class OverlayConsoleCommandAction
{
    None = 0,
    HideConsole,
    ShowConsole
};

class OverlayConsoleCommandTracker
{
public:
    OverlayConsoleCommandTracker() = default;

    void Reset();
    OverlayConsoleCommandAction Update(const InputService& inputService);

private:
    std::string m_commandBuffer{};
};
}
