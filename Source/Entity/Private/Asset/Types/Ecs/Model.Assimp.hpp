#include <EcsComponent/Renderables/3D/Model.hpp>

#include <assimp/Importer.hpp>

namespace Ame::Ecs
{
    class AssImpModelImporter
    {
    public:
        AssImpModelImporter(
            const String& path);

        [[nodiscard]] bool HasMeshes() const;

    public:
        [[nodiscard]] MeshModel::CreateDesc CreateModelDesc(
            Rhi::IRhiDevice* rhiDevice) const;

    private:
        void CreateBufferResources(
            MeshModel::CreateDesc& createDesc,
            Rhi::IRhiDevice*       rhiDevice) const;

        void CreateMaterials(
            MeshModel::CreateDesc& createDesc,
            Rhi::IRhiDevice*       rhiDevice) const;

    private:
        Assimp::Importer m_Importer;
        String           m_ModelRootPath;
    };
} // namespace Ame::Ecs