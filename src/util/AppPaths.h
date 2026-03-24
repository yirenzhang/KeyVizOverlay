#pragma once

#include <filesystem>
#include <string>

namespace keyviz
{
std::filesystem::path GetExecutableDirectory();
std::filesystem::path GetRuntimePath(const std::filesystem::path& relativePath);
std::string GetRuntimePathUtf8(const std::filesystem::path& relativePath);
} // namespace keyviz
