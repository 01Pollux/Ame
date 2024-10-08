#pragma once

#include <Core/Serializable.hpp>
#include <Core/Ame.hpp>
#include <Core/String.hpp>
#include <Core/Coroutine.hpp>

namespace Ame::Asset
{
    // {2FE4DD10-578A-4BF5-8F56-3873DE16BC51}
    inline constexpr UId IID_BaseAssetPackage{
        0x2fe4dd10, 0x578a, 0x4bf5, { 0x8f, 0x56, 0x38, 0x73, 0xde, 0x16, 0xbc, 0x51 }
    };

    // {1DA7809C-D82D-4550-8887-45445C0DDBE5}
    inline constexpr UId IID_MemoryAssetPackage{
        0x1da7809c, 0xd82d, 0x4550, { 0x88, 0x87, 0x45, 0x44, 0x5c, 0xd, 0xdb, 0xe5 }
    };

    // {4AC0D4A3-4CA6-4878-B658-814A2C04026C}
    inline constexpr UId IID_DirectoryAssetPackage{
        0x4ac0d4a3, 0x4ca6, 0x4878, { 0xb6, 0x58, 0x81, 0x4a, 0x2c, 0x4, 0x2, 0x6c }
    };

    //

    // {1BB9A4FA-2503-4525-BD6D-23835181896F}
    inline constexpr UId IID_DataBlobAsset{
        0x1bb9a4fa, 0x2503, 0x4525, { 0xbd, 0x6d, 0x23, 0x83, 0x51, 0x81, 0x89, 0x6f }
    };

    // {22EA167D-E6C2-4A94-8E54-F8F38FACEC25}
    inline constexpr UId IID_TextFileAsset{
        0x22ea167d, 0xe6c2, 0x4a94, { 0x8e, 0x54, 0xf8, 0xf3, 0x8f, 0xac, 0xec, 0x25 }
    };

    //

    // {9A9AC404-D850-4485-8ED3-D90B2818F077}
    inline constexpr UId IID_BaseAssetHandler{
        0x9a9ac404, 0xd850, 0x4485, { 0x8e, 0xd3, 0xd9, 0xb, 0x28, 0x18, 0xf0, 0x77 }
    };

    // {94A0E847-3B48-4EDA-B689-308B39A7A719}
    inline constexpr UId IID_DataBlobAssetHandler{
        0x94a0e847, 0x3b48, 0x4eda, { 0xb6, 0x89, 0x30, 0x8b, 0x39, 0xa7, 0xa7, 0x19 }
    };

    // {39A4BB74-FE17-4C7D-B423-D6893349249C}
    inline constexpr UId IID_TextFileAssetHandler{
        0x39a4bb74, 0xfe17, 0x4c7d, { 0xb4, 0x23, 0xd6, 0x89, 0x33, 0x49, 0x24, 0x9c }
    };

    //

    class IAsset;
    class IAssetPackage;
    class IAssetHandler;
    class Manager;
    class Storage;
} // namespace Ame::Asset
