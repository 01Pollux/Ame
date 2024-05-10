#pragma once

#include <Asset/Pack.hpp>
#include <Asset/Storage.hpp>

namespace Ame::Asset
{
    IAssetPackage::IAssetPackage(
        Storage& Storage) :
        m_Storage(Storage),
        m_Runtime(Storage.GetRuntime())
    {
    }

    Storage& IAssetPackage::GetStorage() const
    {
        return m_Storage;
    }
} // namespace Ame::Asset