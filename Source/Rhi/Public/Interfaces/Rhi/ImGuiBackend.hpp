#pragma once

#include <Core/Interface.hpp>
#include <Rhi/ImGui/ImGuiRenderer.hpp>

namespace Ame::Interfaces
{
    // {CF40EE16-0DE3-4095-938A-FB8993CD01FD}
    inline constexpr UId IID_ImGuiBackend{
        0xcf40ee16, 0xde3, 0x4095, { 0x93, 0x8a, 0xfb, 0x89, 0x93, 0xcd, 0x1, 0xfd }
    };

    class ImGuiBackend final : public BaseObject<IObject>
    {
    public:
        using Base = BaseObject<IObject>;

        IMPLEMENT_QUERY_INTERFACE_IN_PLACE_SUBOJECTS(IID_ImGuiBackend, Base, m_ImGuiRenderer);

    public:
        IMPLEMENT_INTERFACE_CTOR(ImGuiBackend, Ptr<Rhi::IImGuiRenderer> imguiRenderer) :
            Base(counters), m_ImGuiRenderer(std::move(imguiRenderer))
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