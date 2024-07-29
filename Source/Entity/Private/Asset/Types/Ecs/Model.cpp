#include <EcsComponent/Renderables/3D/Model.hpp>

namespace Ame::Ecs
{
    MeshModel* MeshModelLoader::LoadModel(
        const ImportDesc& desc)
    {
        return nullptr;
    }

    MeshModel* MeshModelLoader::CreateModel(
        CreateDesc desc)
    {
        return ObjectAllocator<MeshModel>()(std::move(desc));
    }
} // namespace Ame::Ecs