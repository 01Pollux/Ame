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
            Dg::IRenderDevice* renderDevice) const;

    private:
        void CreateBufferResources(
            MeshModel::CreateDesc& createDesc,
            Dg::IRenderDevice* renderDevice) const;

        void CreateMaterials(
            MeshModel::CreateDesc& createDesc,
            Dg::IRenderDevice*     renderDevice) const;

    private:
        Assimp::Importer m_Importer;
    };
} // namespace Ame::Ecs