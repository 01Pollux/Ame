#include <Asset/Storage.hpp>

#include <Asset/Pack.hpp>
#include <Asset/Packs/Memory.hpp>

#include <Log/Wrapper.hpp>

namespace Ame::Asset
{
    Storage::Storage(
        Co::runtime& Runtime) :
        m_Runtime(Runtime),
        m_Manager(*this, Runtime)
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