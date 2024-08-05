#pragma once

#include <Module/Submodule.hpp>
#include <Rhi/ImGui/ImGuiRenderer.hpp>
#include <Module/Rhi/Config.hpp>

namespace Ame
{
    class ImGuiSubmodule final : public BaseObject<ISubmodule>
    {
    public:
        using Base = BaseObject<ISubmodule>;

        IMPLEMENT_QUERY_INTERFACE_IN_PLACE_SUBOJECTS2(
            IID_ImGuiSubmodule, IID_BaseSubmodule, Base,
            m_ImGuiRenderer);

    public:
        ImGuiSubmodule(
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