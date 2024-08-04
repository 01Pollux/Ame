#pragma once

#include <Module/Submodule.hpp>
#include <Graphics/Renderer/Renderer.hpp>

#include <Module/Graphics/Config.hpp>

namespace Ame::Rhi
{
    class IRhiDevice;
    class IImGuiRenderer;
} // namespace Ame::Rhi

namespace Ame
{
    class RhiModule;

    class RendererSubmodule final : public BaseObject<ISubmodule>
    {
    public:
        using Base = BaseObject<ISubmodule>;

        IMPLEMENT_QUERY_INTERFACE_IN_PLACE_SUBOJECTS2(
            IID_RendererSubmodule, IID_BaseSubmodule, Base,
            m_Renderer);

    public:
        RendererSubmodule(
            IReferenceCounters*         counters,
            RhiModule*                  rhiModule,
            const GraphicsModuleConfig& config);

    public:
        /// <summary>
        /// Returns true if the application should continue running. (the window is not closed)
        /// </summary>
        [[nodiscard]] bool Tick();

    private:
        Ptr<Graphics::Renderer> m_Renderer;
    };
} // namespace Ame