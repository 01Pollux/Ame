#pragma once

#include <Asset/Pack.hpp>
#include <Asset/Storage.hpp>

namespace Ame::Asset
{
    IAssetPackage::IAssetPackage(
        Storage& assetStorage) :
        m_Storage(assetStorage),
        m_Runtime(assetStorage.GetRuntime())
    {
    }

    Storage& IAssetPackage::GetStorage() const
    {
        return m_Storage;
    }
} // namespace Ame::Asset