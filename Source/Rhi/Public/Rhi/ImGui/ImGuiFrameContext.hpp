#pragma once

#include <Rhi/ImGui/ImGuiRenderer.hpp>

namespace Ame
{
    /// <summary>
    /// Helper class to manage the ImGui frame context.
    /// </summary>
    struct ImGuiFrameContext
    {
    public:
        ImGuiFrameContext(
            IImGuiRenderer* renderer = nullptr) :
            m_Renderer(renderer)
        {
        }

        ImGuiFrameContext(
            const ImGuiFrameContext&) = delete;
        ImGuiFrameContext& operator=(
            const ImGuiFrameContext&) = delete;

        ImGuiFrameContext(
            ImGuiFrameContext&& other) noexcept :
            m_Renderer(std::exchange(other.m_Renderer, nullptr))
        {
        }
        ImGuiFrameContext& operator=(
            ImGuiFrameContext&& other) noexcept
        {
            if (this != &other)
            {
                EndFrame();
                m_Renderer = std::exchange(other.m_Renderer, nullptr);
            }

            return *this;
        }

        ~ImGuiFrameContext()
        {
            EndFrame();
        }

    private:
        void BeginFrame()
        {
            if (m_Renderer)
            {
                m_Renderer->BeginFrame();
            }
        }

        void EndFrame()
        {
            if (m_Renderer)
            {
                m_Renderer->EndFrame();
                m_Renderer = nullptr;
            }
        }

    private:
        IImGuiRenderer* m_Renderer = nullptr;
    };
} // namespace Ame