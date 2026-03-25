#include "AppPaths.h"

#include <iterator>

#include <windows.h>

namespace keyviz
{
std::filesystem::path GetExecutableDirectory()
{
    wchar_t modulePath[MAX_PATH] = {};
    const DWORD length = GetModuleFileNameW(nullptr, modulePath, static_cast<DWORD>(std::size(modulePath)));
    if (length == 0 || length >= std::size(modulePath))
    {
        return std::filesystem::current_path();
    }

    std::filesystem::path executablePath(modulePath);
    return executablePath.parent_path();
}

std::filesystem::path GetRuntimePath(const std::filesystem::path& relativePath)
{
    if (relativePath.is_absolute())
    {
        return relativePath;
    }

    return GetExecutableDirectory() / relativePath;
}

std::string GetRuntimePathUtf8(const std::filesystem::path& relativePath)
{
    const std::filesystem::path runtimePath = GetRuntimePath(relativePath);
    return runtimePath.string();
}
} // namespace keyviz
