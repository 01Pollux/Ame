#pragma once

#include <Core/Serializable.hpp>
#include <Core/Ame.hpp>
#include <Core/Guid.hpp>
#include <Core/Coroutine.hpp>

namespace Ame::Asset
{
    // {9A9AC404-D850-4485-8ED3-D90B2818F077}
    static constexpr UId IID_BaseAssetHandler{ 0x9a9ac404, 0xd850, 0x4485, { 0x8e, 0xd3, 0xd9, 0xb, 0x28, 0x18, 0xf0, 0x77 } };

    // {2D815131-9ADF-411D-9CD0-327CD7528898}
    static constexpr UId IID_DefaultAssetHandler{ 0x2d815131, 0x9adf, 0x411d, { 0x9c, 0xd0, 0x32, 0x7c, 0xd7, 0x52, 0x88, 0x98 } };

    class IAsset;
    class IAssetPackage;
    class IAssetHandler;
    class DefaultAssetHandler;
    class Manager;
    class Storage;
} // namespace Ame::Asset
