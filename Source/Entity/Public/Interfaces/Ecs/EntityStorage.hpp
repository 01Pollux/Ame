#pragma once

#include <Core/Interface.hpp>
#include <Ecs/World.hpp>

namespace Ame::Interfaces
{
    // {195A8AE5-5E17-4832-9C41-1B03332A14B2}
    inline constexpr UId IID_EntityStorage{
        0x195a8ae5, 0x5e17, 0x4832, { 0x9c, 0x41, 0x1b, 0x3, 0x33, 0x2a, 0x14, 0xb2 }
    };

    struct EntityStorageCreateDesc;
    class EntityStorage : public BaseObject<IObject>
    {
    public:
        using Base = BaseObject<IObject>;

        IMPLEMENT_QUERY_INTERFACE_IN_PLACE_SUBOJECTS(IID_EntityStorage, Base, m_World);

    private:
        IMPLEMENT_INTERFACE_CTOR(EntityStorage, Ptr<Rhi::IRhiDevice> rhiDevice,
                                 const EntityStorageCreateDesc& storageDesc);

    public:
        [[nodiscard]] Ecs::World* GetWorld() const noexcept
        {
            return m_World;
        }

        [[nodiscard]] bool Tick(double deltaTime);

    private:
        Ptr<Ecs::World> m_World;
    };
} // namespace Ame::Interfaces