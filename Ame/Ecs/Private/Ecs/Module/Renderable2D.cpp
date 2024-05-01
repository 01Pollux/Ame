#include <Ecs/Module/Renderable2D.hpp>
#include <Ecs/Module/Renderable.hpp>

#include <Ecs/Component/Renderable/Renderable.hpp>
#include <Ecs/Component/Renderable/2D/Sprite.hpp>

namespace Ame::Ecs::Module
{
    Renderable2DModule::Renderable2DModule(
        flecs::world& FlecsWorld)
    {
        FlecsWorld.module<Renderable2DModule>("2D");

        RenderableModule::AttachRenderable(FlecsWorld.component<Component::Sprite>());
    }
} // namespace Ame::Ecs::Module