#pragma once

#include <EditorPlugin/EditorCore.hpp>
#include <Plugin/ModuleRegistry.hpp>

#include <Interfaces/Ecs/EntityWorld.hpp>

namespace Ame::Editor::ModuleUtils
{
    [[nodiscard]] inline Ptr<Ecs::World> GetWorld()
    {
        Ptr<Interfaces::IEntityWorld> entityWorld;
        s_ModuleRegistry->RequestInterface(s_ThisPlugin, Interfaces::IID_EntityWorld, entityWorld.DblPtr<IObject>());
        return entityWorld;
    }
}