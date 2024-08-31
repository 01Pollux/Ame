#pragma once

#include <filesystem>

namespace Ame::FileSystem
{
    /// <summary>
    /// Ensure that the directory exists.
    /// </summary>
    inline void EnsureDirectoryExists(const std::filesystem::path& path)
    {
        [[maybe_unused]] std::error_code ErrorCode;
        std::filesystem::create_directories(path, ErrorCode);
    }

    /// <summary>
    /// Ensure that that the path is forward. (No .. indirection)
    /// </summary>
    [[nodiscard]] inline bool EnsurePathIsForward(const std::string& path)
    {
        return !path.contains("..");
    }

    /// <summary>
    /// Ensure that that the path is forward. (No .. indirection)
    /// </summary>
    [[nodiscard]] inline bool EnsurePathIsForward(const std::wstring& path)
    {
        return !path.contains(L"..");
    }

    /// <summary>
    /// Ensure that that the path is forward. (No .. indirection)
    /// </summary>
    [[nodiscard]] inline bool EnsurePathIsForward(const std::filesystem::path& path)
    {
        return EnsurePathIsForward(path.native());
    }

} // namespace Ame::FileSystem