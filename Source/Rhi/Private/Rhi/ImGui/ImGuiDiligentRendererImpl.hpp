#pragma once

#ifndef AME_DIST
#include <map>
#include <imgui.h>

#include <Rhi/ImGui/ImGuiRenderer.hpp>
#include <Rhi/ImGui/ImGuiRendererCreateDesc.hpp>
#include <Window/Window.hpp>

namespace Ame::Rhi
{
    class ImGuiDiligentRendererImpl final : public BaseObject<IImGuiRenderer>
    {
    public:
        using Base = BaseObject<IImGuiRenderer>;

        IMPLEMENT_QUERY_INTERFACE_IN_PLACE(
            IID_ImGuiRenderer, Base);

    public:
        ImGuiDiligentRendererImpl(
            IReferenceCounters*            counters,
            const ImGuiRendererCreateDesc& desc);

        ~ImGuiDiligentRendererImpl() override;

    public:
        void BeginFrame() override;
        void EndFrame() override;
        void SubmitDrawData() override;
        void Reset() override;

    public:
        ImFont* AME_METHOD(FindFont)(const String& fontName) const override;
        ImFont* AME_METHOD(LoadFont)(const String& fontName, const ImFontConfig& fontConfig) override;
        ImFont* AME_METHOD(LoadCompressedFont)(const char* fontName, const ImFontConfig& fontConfig) override;

    private:
        void InvalidateDeviceObjects();
        void CreateDeviceObjects();
        void CreateFontsTextures();

    private:
        Ptr<Dg::IRenderDevice>  m_RenderDevice;
        Ptr<Dg::IDeviceContext> m_DeviceContext;
        Ptr<Window::IWindow>    m_Window;

        Ptr<Dg::IBuffer> m_VertexBuffer;
        Ptr<Dg::IBuffer> m_IndexBuffer;

        Ptr<Dg::IPipelineState> m_PipelineState;

        Ptr<Dg::IBuffer>                m_TransformBuffer;
        Ptr<Dg::ITextureView>           m_FontTextureView;
        Ptr<Dg::IShaderResourceBinding> m_SRB;

        Dg::TEXTURE_FORMAT m_BackBufferFormat  = {};
        Dg::TEXTURE_FORMAT m_DepthBufferFormat = {};

        ImGuiContext*             m_Context = nullptr;
        std::map<String, ImFont*> m_Fonts;

        ImGuiColorConversionMode m_ConversionMode;

        bool m_MultiThreaded       : 1 = false;
        bool m_BaseVertexSupported : 1 = false;
    };
} // namespace Ame::Rhi
#endif
