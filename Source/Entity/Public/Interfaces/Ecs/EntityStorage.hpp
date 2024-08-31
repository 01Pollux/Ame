#pragma once

#include <Interfaces/Ecs/Config.hpp>
#include <Ecs/World.hpp>

namespace Ame::Interfaces
{
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