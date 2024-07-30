#pragma once

#include <EcsComponent/Renderables/3D/Model.hpp>

namespace Ame::Ecs
{
    struct MeshModelLoader
    {
        struct ImportDesc
        {
            Dg::IRenderDevice* RenderDevice = nullptr;
            String             ModelPath;
        };

        using CreateDesc = MeshModel::CreateDesc;

        static MeshModel* LoadModel(
            const ImportDesc& desc);

        static MeshModel* CreateModel(
            CreateDesc desc);
    };
} // namespace Ame::Ecs