#include <Asset/Storage.hpp>

#include <Asset/Pack.hpp>
#include <Asset/Packs/Memory.hpp>

#include <Log/Wrapper.hpp>

namespace Ame::Asset
{
    Storage::Storage(
        Co::runtime& coroutine) :
        m_Runtime(coroutine),
        m_Manager(*this, coroutine)
    {
        Mount<MemoryAssetPackage>();
    }

    Storage::~Storage() = default;

    //

    Manager& Storage::GetManager()
    {
        return m_Manager;
    }

    Co::runtime& Storage::GetRuntime() const
    {
        return m_Runtime;
    }
} // namespace Ame::Asset