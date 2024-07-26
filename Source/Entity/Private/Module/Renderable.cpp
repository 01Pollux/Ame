#include <Ecs/Module/Renderable.hpp>

#include <Ecs/Tag/VisibleToCamera.hpp>

#include <Ecs/Component/Renderable/BaseRenderable.hpp>
#include <Ecs/Module/Renderable2D.hpp>

namespace Ame::Ecs::Module
{
    RenderableModule::RenderableModule(
        flecs::world& flecsWorld)
    {
        flecsWorld.module<RenderableModule>();

        flecsWorld.component<Component::BaseRenderable>();
        flecsWorld.component<Component::StandardBatchedRenderable>();
        flecsWorld.component<Tag::VisibleToCamera>();

        flecsWorld.import <Renderable2DModule>();

        //

        RegisterVisbilityObserver(flecsWorld);
    }
} // namespace Ame::Ecs::Module