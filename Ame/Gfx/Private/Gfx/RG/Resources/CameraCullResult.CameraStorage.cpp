#include <Gfx/RG/Resources/CameraCullResult.hpp>

#include <Ecs/Component/Renderable/BaseRenderable.hpp>

namespace Ame::Gfx::RG
{
    CameraCullResult::CameraStorage::CameraStorage(
        Rhi::Device&                           RhiDevice,
        const Rhi::Util::BlockBasedBufferDesc& VertexDesc,
        const Rhi::Util::BlockBasedBufferDesc& IndexDesc,
        const Rhi::Util::SlotBasedBufferDesc&  InstanceDesc) :
        DynamicVertices(RhiDevice, VertexDesc),
        DynamicIndices(RhiDevice, IndexDesc),
        AllInstances(RhiDevice, InstanceDesc)
    {
    }

    void CameraCullResult::CameraStorage::Reset()
    {
        DynamicVertices.Reset();
        DynamicIndices.Reset();
        AllInstances.Reset();
    }

    void CameraCullResult::CameraStorage::Flush()
    {
        DynamicVertices.FlushAll();
        DynamicIndices.FlushAll();
        AllInstances.Flush();
    }
} // namespace Ame::Gfx::RG