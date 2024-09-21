#pragma once

#include <Asset/Pack.hpp>
#include <Asset/Storage.hpp>

namespace Ame::Asset
{
    IAssetPackage::IAssetPackage(IReferenceCounters* counters, Storage& assetStorage) :
        IObjectWithCallback(counters), m_Storage(assetStorage), m_Runtime(assetStorage.GetRuntime())
    {
    }

    Storage& IAssetPackage::GetStorage() const
    {
        return m_Storage;
    }
} // namespace Ame::Asset