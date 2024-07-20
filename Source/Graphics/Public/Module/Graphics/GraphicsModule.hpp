#pragma once

#include <Module/Module.hpp>

#include <Module/Graphics/ImGuiSubmodule.hpp>

namespace Ame
{
    // {B8B1F60A-BFB3-424B-9AA4-1D2D844B091F}
    static const UId IID_GraphicsModule = { 0xb8b1f60a, 0xbfb3, 0x424b, { 0x9a, 0xa4, 0x1d, 0x2d, 0x84, 0x4b, 0x9, 0x1f } };

    class GraphicsModule final : public BaseObject<IModule>
    {
    public:
        using Base = BaseObject<IModule>;

        IMPLEMENT_QUERY_INTERFACE_IN_PLACE_SUBOJECTS2(
            IID_GraphicsModule, IID_BaseModule, Base,
            m_ImGuiSubmodule)

    public:
        GraphicsModule(
            IReferenceCounters* counters) :
            Base(counters, IID_GraphicsModule),
            m_ImGuiSubmodule(ObjectAllocator<ImGuiSubmodule>(this)())
        {
        }

    private:
        Ptr<ImGuiSubmodule> m_ImGuiSubmodule;
    }; // namespace Ame
} // namespace Ame