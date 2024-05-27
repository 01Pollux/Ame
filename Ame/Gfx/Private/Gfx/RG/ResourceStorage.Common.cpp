#include <Gfx/RG/ResourceStorage.hpp>
#include <Gfx/RG/Resources/CoreResources.hpp>
#include <Gfx/RG/Resources/Names.hpp>

#include <Log/Wrapper.hpp>

namespace Ame::Gfx::RG
{
    const ResourceHandle& ResourceStorage::GetFrameResource() const
    {
        return *GetResource(Names::c_FrameResource);
    }

    const Rhi::ResourceView& ResourceStorage::GetFrameResourceHandle() const
    {
        return GetResourceViewHandle(Names::c_FrameResourceMainView);
    }

    const FrameResourceCPU& ResourceStorage::GetFrameResourceData() const
    {
        return m_CoreResources->GetFrameResourceData();
    }
} // namespace Ame::Gfx::RG