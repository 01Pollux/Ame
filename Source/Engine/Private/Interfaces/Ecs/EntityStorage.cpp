#include <Interfaces/Ecs/Config.hpp>
#include <Interfaces/Ecs/EntityWorld.hpp>

#include <EcsModule/RenderableModule.hpp>
#include <EcsModule/Renderable3DModule.hpp>

#include <Rhi/Device/RhiDevice.hpp>

namespace Ame::Interfaces
{
    Ptr<IEntityWorld> CreateEntityWorld(Ptr<Rhi::IRhiDevice> rhiDevice, const EntityWorldCreateDesc& createDesc)
    {
        auto world = AmeCreate(Ecs::World, std::move(rhiDevice));

#ifndef AME_DIST
        if (createDesc.EnableMonitors)
        {
            (*world)->emplace<flecs::Rest>();
        }
#endif

        bool anyRenderable = createDesc.Enable3DModule;
        if (anyRenderable)
        {
            world->ImportModule<Ecs::RenderableEcsModule>();
            if (createDesc.Enable3DModule)
            {
                world->ImportModule<Ecs::Renderable3DEcsModule>();
            }
        }

        return world;
    }
} // namespace Ame::Interfaces
