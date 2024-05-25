#include <Gfx/RG/Resources/CameraCullResult.hpp>

#include <Ecs/Component/Renderable/BaseRenderable.hpp>

namespace Ame::Gfx::RG
{
    CameraCullResult::CameraStorage::CameraStorage(
        Rhi::Device&                           rhiDevice,
        const Rhi::Util::BlockBasedBufferDesc& vertexDesc,
        const Rhi::Util::BlockBasedBufferDesc& indexDesc,
        const Rhi::Util::SlotBasedBufferDesc&  instanceDesc) :
        DynamicVertices(rhiDevice, vertexDesc),
        DynamicIndices(rhiDevice, indexDesc),
        AllInstances(rhiDevice, instanceDesc)
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