#pragma once

#include <Module/Module.hpp>

#include <Module/Graphics/Config.hpp>
#include <Module/Graphics/RendererSubmodule.hpp>

namespace Ame
{
    class GraphicsModule final : public BaseObject<IModule>
    {
    public:
        using Base = BaseObject<IModule>;

        IMPLEMENT_QUERY_INTERFACE_IN_PLACE_SUBOJECTS2(
            IID_GraphicsModule, IID_BaseModule, Base,
            m_RendererSubmodule);

        struct Dependencies
        {
            RhiModule*    RhiMod;
            EntityModule* EntMod;
        };

    public:
        GraphicsModule(
            IReferenceCounters*         counters,
            Dependencies                dependencies,
            const GraphicsModuleConfig& config);

    private:
        Ptr<RendererSubmodule> m_RendererSubmodule;
    }; // namespace Ame
} // namespace Ame