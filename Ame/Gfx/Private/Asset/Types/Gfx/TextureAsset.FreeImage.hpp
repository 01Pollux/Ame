#pragma once

#include <Asset/Types/Gfx/TextureAsset.hpp>
#include <FreeImage/FreeImage.hpp>

namespace Ame::Asset::Gfx
{
    /// <summary>
    /// Converts a ImageFormat to a FreeImage format
    /// </summary>
    [[nodiscard]] ImageFormat FreeImageToImageFormat(
        FREE_IMAGE_FORMAT format);
} // namespace Ame::Asset::Gfx