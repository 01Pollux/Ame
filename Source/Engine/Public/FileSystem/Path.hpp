#pragma once

#include <filesystem>

namespace Ame::FileSystem
{
    /// <summary>
    /// Convert the path to a unix path.
    /// </summary>
    inline void MakeUnixPath(std::filesystem::path& path)
    {
#if AME_PLATFORM_WINDOWS
        auto str = std::move(path.native());
        std::replace(str.begin(), str.end(), L'\\', L'/');
        path = std::move(str);
#endif
    }

    /// <summary>
    /// Convert the path to a unix path.
    /// </summary>
    [[nodiscard]] inline std::filesystem::path ConvertToUnixPath(const std::filesystem::path& path)
    {
#if AME_PLATFORM_WINDOWS
        auto copy = path;
        MakeUnixPath(copy);
        return copy;
#else
        return path;
#endif
    }
} // namespace Ame::FileSystem