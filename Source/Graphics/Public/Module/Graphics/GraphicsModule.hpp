#pragma once

#include <Module/Module.hpp>

#include <Module/Graphics/Config.hpp>
#include <Module/Graphics/PlatformWindowSubmodule.hpp>
#include <Module/Graphics/RhiGraphicsSubmodule.hpp>
#include <Module/Graphics/RendererSubmodule.hpp>
#include <Module/Graphics/ImGuiSubmodule.hpp>

namespace Ame
{
    class GraphicsModule final : public BaseObject<IModule>
    {
    public:
        using Base = BaseObject<IModule>;

        IMPLEMENT_QUERY_INTERFACE_IN_PLACE_SUBOJECTS2(
            IID_GraphicsModule, IID_BaseModule, Base,
            m_PlatformWindowSubmodule,
            m_RhiGraphicsSubmodule,
            m_RendererSubmodule,
            m_ImGuiSubmodule);

    public:
        GraphicsModule(
            IReferenceCounters*         counters,
            const GraphicsModuleConfig& config);

    private:
        Ptr<PlatformWindowSubmodule> m_PlatformWindowSubmodule;
        Ptr<RhiGraphicsSubmodule>    m_RhiGraphicsSubmodule;
        Ptr<RendererSubmodule>       m_RendererSubmodule;
        Ptr<ImGuiSubmodule>          m_ImGuiSubmodule;
    }; // namespace Ame
} // namespace Ame