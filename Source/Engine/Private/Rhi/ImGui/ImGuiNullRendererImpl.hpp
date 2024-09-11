#pragma once

#include <Rhi/ImGui/ImGuiRenderer.hpp>

namespace Ame::Rhi
{
    class ImGuiNullRendererImpl final : public BaseObject<IImGuiRenderer>
    {
    public:
        using Base = BaseObject<IImGuiRenderer>;

        IMPLEMENT_QUERY_INTERFACE2_IN_PLACE(IID_ImGuiNullRenderer, IID_ImGuiRenderer, Base);

    public:
        IMPLEMENT_INTERFACE_CTOR(ImGuiNullRendererImpl) : Base(counters)
        {
        }

    public:
        void BeginFrame(Dg::SURFACE_TRANSFORM) override
        {
        }
        void EndFrame() override
        {
        }
        void Reset() override
        {
        }

        ImFont* FindFont(const String&) const override
        {
            return nullptr;
        }
        ImFont* LoadFont(const String&, const ImFontConfig&) override
        {
            return nullptr;
        }
        ImFont* LoadCompressedFont(const char*, const ImFontConfig&) override
        {
            return nullptr;
        }

        ImGuiContext* GetContext() const
        {
            return nullptr;
        }

        bool RenderBackbufferToTexture() const override
        {
            return false;
        }
    };
} // namespace Ame::Rhi