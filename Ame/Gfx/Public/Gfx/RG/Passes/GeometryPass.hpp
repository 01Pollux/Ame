#pragma once

#include <Gfx/RG/Pass.hpp>

#include <Ecs/Universe.hpp>
#include <Gfx/Ecs/Component/RenderInstance.hpp>
#include <Gfx/Draw/Batcher/Batcher.hpp>

namespace Ame::Gfx::RG::Std
{
    class GeometryPass : public Pass
    {
    public:
        GeometryPass(
            Ecs::Universe& Universe);

    private:
        /// <summary>
        /// Create the render rules.
        /// </summary>
        void CreateRenderRules();

    private:
        Ref<Ecs::Universe> m_Universe;

        // TODO: this should be in seperate pass where we're going to be using it for depth prepass, indirect command composer, light pass, etc.
        using Render2DRuleType = Ecs::Rule<const Ecs::Gfx::Component::RenderInstance>;

        Render2DRuleType m_Render2DRule;
        DrawBatcher      m_DrawBatcher;
    };
} // namespace Ame::Gfx::RG::Std