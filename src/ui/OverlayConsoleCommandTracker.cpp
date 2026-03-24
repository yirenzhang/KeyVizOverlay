#include "OverlayConsoleCommandTracker.h"

#include "input/InputService.h"

namespace keyviz
{
namespace
{
constexpr std::size_t kCommandBufferLimit = 24U;
constexpr const char* kHideCommand = "hidehide";
constexpr const char* kShowCommand = "showshow";

bool EndsWithCommand(const std::string& text, const char* suffix)
{
    const std::size_t suffixLength = std::char_traits<char>::length(suffix);
    if (text.size() < suffixLength)
    {
        return false;
    }

    return text.compare(text.size() - suffixLength, suffixLength, suffix) == 0;
}

void AppendCommandLetter(const InputService& inputService, std::uint32_t keyCode, char letter, std::string& commandBuffer)
{
    if (!inputService.WasPressedThisFrame(keyCode))
    {
        return;
    }

    commandBuffer.push_back(letter);
    if (commandBuffer.size() > kCommandBufferLimit)
    {
        commandBuffer.erase(0, commandBuffer.size() - kCommandBufferLimit);
    }
}
}

void OverlayConsoleCommandTracker::Reset()
{
    m_commandBuffer.clear();
}

OverlayConsoleCommandAction OverlayConsoleCommandTracker::Update(const InputService& inputService)
{
    bool hasLetterPressedThisFrame = false;
    bool hasNonCommandLetterPressedThisFrame = false;
    for (std::uint32_t keyCode = 'A'; keyCode <= 'Z'; ++keyCode)
    {
        if (!inputService.WasPressedThisFrame(keyCode))
        {
            continue;
        }

        hasLetterPressedThisFrame = true;
        const char letter = static_cast<char>(keyCode + ('a' - 'A'));
        const bool isCommandLetter = letter == 'h' || letter == 'i' || letter == 'd' ||
            letter == 'e' || letter == 's' || letter == 'o' || letter == 'w';
        if (!isCommandLetter)
        {
            hasNonCommandLetterPressedThisFrame = true;
            break;
        }
    }

    if (hasNonCommandLetterPressedThisFrame)
    {
        m_commandBuffer.clear();
        return OverlayConsoleCommandAction::None;
    }

    if (!hasLetterPressedThisFrame)
    {
        return OverlayConsoleCommandAction::None;
    }

    AppendCommandLetter(inputService, 'H', 'h', m_commandBuffer);
    AppendCommandLetter(inputService, 'I', 'i', m_commandBuffer);
    AppendCommandLetter(inputService, 'D', 'd', m_commandBuffer);
    AppendCommandLetter(inputService, 'E', 'e', m_commandBuffer);
    AppendCommandLetter(inputService, 'S', 's', m_commandBuffer);
    AppendCommandLetter(inputService, 'O', 'o', m_commandBuffer);
    AppendCommandLetter(inputService, 'W', 'w', m_commandBuffer);

    if (EndsWithCommand(m_commandBuffer, kHideCommand))
    {
        m_commandBuffer.clear();
        return OverlayConsoleCommandAction::HideConsole;
    }

    if (EndsWithCommand(m_commandBuffer, kShowCommand))
    {
        m_commandBuffer.clear();
        return OverlayConsoleCommandAction::ShowConsole;
    }

    return OverlayConsoleCommandAction::None;
}
}
