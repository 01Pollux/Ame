#pragma once

#include <EcsComponent/Renderables/BaseRenderable3D.hpp>

namespace Ame::Ecs
{
    // TODO
    class StaticMesh : public IBaseRenderable3D
    {
    public:
        void Serialize(BinaryOArchiver& ar) const override
        {
        }

        void Deserialize(BinaryIArchiver& ar) override
        {
        }
    };

    struct StaticMeshComponent
    {
        Ptr<StaticMesh> Object;
    };
} // namespace Ame::Ecs