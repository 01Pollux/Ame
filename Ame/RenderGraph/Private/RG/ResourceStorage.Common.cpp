#include <RG/ResourceStorage.hpp>
#include <RG/Resources/CoreResources.hpp>
#include <RG/Resources/Names.hpp>

#include <Log/Wrapper.hpp>

namespace Ame::RG
{
    const ResourceHandle& ResourceStorage::GetFrameResource() const
    {
        return *GetResource(Names::c_FrameResource);
    }

    const Rhi::ResourceView& ResourceStorage::GetFrameResourceHandle() const
    {
        return *GetResourceViewHandle(Names::c_FrameResourceMainView);
    }

    const FrameResourceCPU& ResourceStorage::GetFrameResourceData() const
    {
        return m_CoreResources->GetFrameResourceData();
    }
} // namespace Ame::RG