#include <EcsComponent/Renderables/3D/Model.hpp>
#include <Asset/Types/Ecs/Model.Assimp.hpp>

namespace Ame::Ecs
{
    MeshModel* MeshModelLoader::LoadModel(
        const ImportDesc& desc)
    {
        AssImpModelImporter importer(desc.Path);
        return ObjectAllocator<MeshModel>()(importer.CreateModelDesc(desc.RenderDevice));
    }

    MeshModel* MeshModelLoader::CreateModel(
        CreateDesc desc)
    {
        return ObjectAllocator<MeshModel>()(std::move(desc));
    }
} // namespace Ame::Ecs