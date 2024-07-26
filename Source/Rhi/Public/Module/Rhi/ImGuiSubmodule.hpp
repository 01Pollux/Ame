#pragma once

#include <Module/Submodule.hpp>
#include <Rhi/ImGui/ImGuiRenderer.hpp>

namespace Ame
{
    // {CF40EE16-0DE3-4095-938A-FB8993CD01FD}
    static constexpr UId IID_ImGuiSubmodule = { 0xcf40ee16, 0xde3, 0x4095, { 0x93, 0x8a, 0xfb, 0x89, 0x93, 0xcd, 0x1, 0xfd } };

    class ImGuiSubmodule final : public BaseObject<ISubmodule>
    {
    public:
        using Base = BaseObject<ISubmodule>;

        IMPLEMENT_QUERY_INTERFACE_IN_PLACE_SUBOJECTS2(
            IID_ImGuiSubmodule, IID_BaseSubmodule, Base,
            m_ImGuiRenderer);

    public:
        explicit ImGuiSubmodule(
            IReferenceCounters*      counters,
            Ptr<Rhi::IImGuiRenderer> imguiRenderer) :
            Base(counters, IID_ImGuiSubmodule),
            m_ImGuiRenderer(std::move(imguiRenderer))
        {
        }

    private:
        Ptr<Rhi::IImGuiRenderer> m_ImGuiRenderer;
    };
} // namespace Ame