#pragma once

#include <EcsComponent/Renderables/3D/Model.hpp>
#include <Core/Coroutine.hpp>

namespace Ame::Ecs
{
    struct MeshModelLoader
    {
        struct ImportDesc
        {
            Rhi::IRhiDevice* RhiDevice = nullptr;
            String           ModelPath;
        };

        using CreateDesc = MeshModel::CreateDesc;

        [[nodiscard]] static MeshModel* CreateModel(
            CreateDesc desc);

        [[nodiscard]] static MeshModel* LoadModel(
            const ImportDesc& desc);

        [[nodiscard]] static Co::result<MeshModel*> LoadModelAsync(
            const ImportDesc& desc);
    };
} // namespace Ame::Ecs