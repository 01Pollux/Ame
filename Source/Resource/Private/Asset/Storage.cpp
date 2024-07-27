#include <Asset/Storage.hpp>

#include <Asset/Pack.hpp>
#include <Asset/Packs/Memory.hpp>

namespace Ame::Asset
{
    Storage::Storage(
        SharedPtr<Co::runtime> coroutine) :
        m_Runtime(std::move(coroutine)),
        m_Manager(*this)
    {
        Mount<MemoryAssetPackage>();
        RegisterHandler<DefaultAssetHandler>();
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