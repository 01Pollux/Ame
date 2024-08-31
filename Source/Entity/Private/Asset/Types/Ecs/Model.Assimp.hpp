#include <EcsComponent/Renderables/3D/Model.hpp>

#include <assimp/Importer.hpp>
#include <Core/Coroutine.hpp>

namespace Ame::Ecs
{
    class AssImpModelImporter
    {
    public:
        AssImpModelImporter(const String& path);

    public:
        [[nodiscard]] MeshModel::CreateDesc CreateModelDesc(Rhi::IRhiDevice* rhiDevice) const;

        [[nodiscard]] Co::result<MeshModel::CreateDesc> CreateModelDescAsync(Rhi::IRhiDevice* rhiDevice) const;

    private:
        void CreateBufferResources(MeshModel::CreateDesc& createDesc, Rhi::IRhiDevice* rhiDevice) const;

        void CreateMaterials(MeshModel::CreateDesc& createDesc, Rhi::IRhiDevice* rhiDevice) const;

    private:
        Assimp::Importer m_Importer;
        String           m_ModelRootPath;
    };
} // namespace Ame::Ecs