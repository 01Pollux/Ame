#pragma once

#include <FreeImage.h>
#include <Core/String.hpp>

namespace Ame::Extensions
{
    using FreeImageBitMapUPtr = std::unique_ptr<FIBITMAP, decltype([](FIBITMAP* bitmap)
                                                                   { FreeImage_Unload(bitmap); })>;
} // namespace Ame::Extensions