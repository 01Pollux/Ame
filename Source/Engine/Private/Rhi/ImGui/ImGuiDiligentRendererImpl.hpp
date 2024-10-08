#pragma once

#ifndef AME_NO_IMGUI
#include <map>
#include <imgui.h>

#include <Rhi/ImGui/ImGuiRenderer.hpp>
#include <Rhi/ImGui/ImGuiRendererCreateDesc.hpp>

#include <Window/Window.hpp>

namespace Ame::Rhi
{
    class ImGuiDiligentRendererImpl final : public IImGuiRenderer
    {
    public:
        IMPLEMENT_QUERY_INTERFACE_IN_PLACE(IID_ImGuiRenderer, IImGuiRenderer);

    private:
        IMPLEMENT_INTERFACE_CTOR(ImGuiDiligentRendererImpl, const ImGuiRendererCreateDesc& desc);

    public:
        ~ImGuiDiligentRendererImpl() override;

    public:
        void BeginFrame(Dg::SURFACE_TRANSFORM transform) override;
        void EndFrame() override;
        void Reset() override;

    public:
        ImFont* FindFont(const String& fontName) const override;
        ImFont* LoadFont(const String& fontName, const ImFontConfig& fontConfig) override;
        ImFont* LoadCompressedFont(const char* fontName, const ImFontConfig& fontConfig) override;

        bool RenderBackbufferToTexture() const override;

    private:
        void InitializeImGui();
        void NewFrameImGui();
        void ShutdownImGui();

    private:
        void SetDefaultTheme();
        void LoadDefaultFonts();

    private:
        void InvalidateDeviceObjects();
        void CreateDeviceObjects();
        void CreateFontsTextures();

    private:
        void SubmitDrawData(ImDrawData* drawData);

    private:
        Ptr<Dg::IRenderDevice>  m_RenderDevice;
        Ptr<Dg::IDeviceContext> m_DeviceContext;
        Ptr<Dg::ISwapChain>     m_Swapchain;
        Ptr<Window::IWindow>    m_Window;

        size_t m_VertexBufferSize = 0;
        size_t m_IndexBufferSize  = 0;

        Ptr<Dg::IBuffer> m_VertexBuffer;
        Ptr<Dg::IBuffer> m_IndexBuffer;

        Ptr<Dg::IPipelineState>          m_PipelineState;
        Ptr<Dg::IBuffer>                 m_TransformBuffer;
        Ptr<Dg::ITextureView>            m_FontTextureView;
        Ptr<Dg::IShaderResourceBinding>  m_SRB;
        Ptr<Dg::IShaderResourceVariable> m_TextureVariable;

        Dg::TEXTURE_FORMAT    m_BackBufferFormat  = {};
        Dg::TEXTURE_FORMAT    m_DepthBufferFormat = {};
        Dg::SURFACE_TRANSFORM m_Transform         = Dg::SURFACE_TRANSFORM_IDENTITY;

        std::map<String, ImFont*> m_Fonts;

        ImGuiColorConversionMode m_ConversionMode;

        bool m_BaseVertexSupported : 1 = false;
    };
} // namespace Ame::Rhi
#endif
