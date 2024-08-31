#pragma once

#include <Rhi/ImGui/ImGuiRenderer.hpp>
#include <Interfaces/Rhi/Config.hpp>

namespace Ame::Interfaces
{
    class ImGuiBackend final : public BaseObject<IObject>
    {
    public:
        using Base = BaseObject<IObject>;

        IMPLEMENT_QUERY_INTERFACE_IN_PLACE_SUBOJECTS(
            IID_ImGuiBackend, Base,
            m_ImGuiRenderer);

    public:
        IMPLEMENT_INTERFACE_CTOR(ImGuiBackend, Ptr<Rhi::IImGuiRenderer> imguiRenderer) :
            Base(counters),
            m_ImGuiRenderer(std::move(imguiRenderer))
        {
        }

    public:
        [[nodiscard]] Rhi::IImGuiRenderer* GetImGuiRenderer() const noexcept
        {
            return m_ImGuiRenderer;
        }

    private:
        Ptr<Rhi::IImGuiRenderer> m_ImGuiRenderer;
    };
} // namespace Ame::Interfaces