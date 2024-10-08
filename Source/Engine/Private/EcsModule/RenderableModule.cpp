#include <EcsModule/RenderableModule.hpp>

#include <EcsComponent/Renderables/BaseRenderable.hpp>

namespace Ame::Ecs
{
    RenderableEcsModule::RenderableEcsModule(flecs::world& flecsWorld)
    {
        flecsWorld.module<RenderableEcsModule>();

        flecsWorld.component<IBaseRenderableComponent>();
    }
} // namespace Ame::Ecs