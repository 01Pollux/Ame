#include <EcsComponent/Renderables/3D/ModelLoader.hpp>
#include <Asset/Types/Ecs/Model.Assimp.hpp>

namespace Ame::Ecs
{
    MeshModel* MeshModelLoader::LoadModel(
        const ImportDesc& desc)
    {
        AssImpModelImporter importer(desc.ModelPath);
        return ObjectAllocator<MeshModel>()(importer.CreateModelDesc(desc.RhiDevice));
    }

    MeshModel* MeshModelLoader::CreateModel(
        CreateDesc desc)
    {
        return ObjectAllocator<MeshModel>()(std::move(desc));
    }
} // namespace Ame::Ecs