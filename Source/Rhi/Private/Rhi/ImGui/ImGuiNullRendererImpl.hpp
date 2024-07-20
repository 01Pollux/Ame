#pragma once

#include <Rhi/ImGui/ImGuiRenderer.hpp>

namespace Ame::Rhi
{
    class ImGuiNullRendererImpl final : public BaseObject<IImGuiRenderer>
    {
    public:
        using Base = BaseObject<IImGuiRenderer>;

        IMPLEMENT_QUERY_INTERFACE_IN_PLACE(
            IID_ImGuiNullRenderer, Base);

    public:
        ImGuiNullRendererImpl(
            IReferenceCounters* referenceCounters) :
            Base(referenceCounters)
        {
        }

    public:
        void BeginFrame() override
        {
        }
        void EndFrame() override
        {
        }
        void SubmitDrawData() override
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
    };
} // namespace Ame::Rhi