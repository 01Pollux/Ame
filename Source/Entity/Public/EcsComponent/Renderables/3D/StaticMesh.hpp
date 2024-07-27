#pragma once

#include <EcsComponent/Renderables/BaseRenderable3D.hpp>

namespace Ame::Ecs
{
    // {75B8DFC4-B5C5-4991-8230-D37C53E814FC}
    static constexpr UId IID_StaticMesh{ 0x75b8dfc4, 0xb5c5, 0x4991, { 0x82, 0x30, 0xd3, 0x7c, 0x53, 0xe8, 0x14, 0xfc } };

    // TODO
    class StaticMesh : public BaseObject<IBaseRenderable3D>
    {
    public:
        using Base = BaseObject<IBaseRenderable3D>;

        IMPLEMENT_QUERY_INTERFACE_IN_PLACE(
            IID_StaticMesh, Base);

        StaticMesh(
            IReferenceCounters* counters) :
            Base(counters)
        {
        }

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

        StaticMeshComponent(
            Ptr<StaticMesh> object = {}) :
            Object(std::move(object))
        {
        }
    };
} // namespace Ame::Ecs