#include <EcsComponent/Renderables/3D/ModelLoader.hpp>
#include <Asset/Types/Ecs/Model.Assimp.hpp>

namespace Ame::Ecs
{
    MeshModel* MeshModelLoader::CreateModel(
        CreateDesc desc)
    {
        return ObjectAllocator<MeshModel>()(std::move(desc));
    }

    MeshModel* MeshModelLoader::LoadModel(
        const ImportDesc& desc)
    {
        AssImpModelImporter importer(desc.ModelPath);
        return CreateModel(importer.CreateModelDesc(desc.RhiDevice));
    }

    Co::result<MeshModel*> MeshModelLoader::LoadModelAsync(
        const ImportDesc& desc)
    {
        AssImpModelImporter importer(desc.ModelPath);
        co_return CreateModel(co_await importer.CreateModelDescAsync(desc.RhiDevice));
    }
} // namespace Ame::Ecs