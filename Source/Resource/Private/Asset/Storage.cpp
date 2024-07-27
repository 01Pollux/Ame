#include <Asset/Storage.hpp>

#include <Asset/Pack.hpp>
#include <Asset/Packs/Memory.hpp>

#include <Asset/Types/Common/DataBlob.hpp>
#include <Asset/Types/Common/TextFile.hpp>

namespace Ame::Asset
{
    Storage::Storage(
        SharedPtr<Co::runtime> coroutine) :
        m_Runtime(std::move(coroutine)),
        m_Manager(*this)
    {
        Mount<MemoryAssetPackage>();
        RegisterHandler<DefaultAssetHandler<Common::DataBlobAsset, IID_DataBlobAssetHandler>>();
        RegisterHandler<DefaultAssetHandler<Common::TextFileAsset, IID_TextFileAssetHandler>>();
    }

    Storage::~Storage() = default;

    //

    Manager& Storage::GetManager()
    {
        return m_Manager;
    }

    Co::runtime& Storage::GetRuntime() const
    {
        return *m_Runtime;
    }
} // namespace Ame::Asset