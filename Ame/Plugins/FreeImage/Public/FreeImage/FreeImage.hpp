#pragma once

#include <FreeImage/Core.hpp>

namespace Ame::Plugins
{
    class FreeImageInstance
    {
    public:
        using BitmapUPtr = FreeImageBitMapUPtr;

    public:
        FreeImageInstance();

        FreeImageInstance(const FreeImageInstance&) = delete;
        FreeImageInstance(FreeImageInstance&&)      = delete;

        FreeImageInstance& operator=(const FreeImageInstance&) = delete;
        FreeImageInstance& operator=(FreeImageInstance&&)      = delete;

        ~FreeImageInstance();

    public:
        /// <summary>
        /// A FreeImageIO struct that is used to read and write images
        /// </summary>
        [[nodiscard]] static FreeImageIO GetIO() noexcept;

        /// <summary>
        /// Get format from extension
        /// </summary>
        [[nodiscard]] static FREE_IMAGE_FORMAT FormatFromExtension(
            const String& extension);
    };
} // namespace Ame::Plugins