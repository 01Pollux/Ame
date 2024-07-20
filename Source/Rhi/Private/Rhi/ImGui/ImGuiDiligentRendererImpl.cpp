#ifndef AME_DIST

#include <mutex>

#include <Rhi/ImGui/ImGuiDiligentRendererImpl.hpp>
#include <Rhi/ImGui/ImGuiRendererCreateDesc.hpp>

#include <DiligentCore/Graphics/GraphicsTools/interface/CommonlyUsedStates.h>
#include <DiligentCore/Graphics/GraphicsTools/interface/GraphicsUtilities.h>

#include <Rhi/Device/RhiDevice.hpp>
#include <Math/Matrix.hpp>

#include <Log/Wrapper.hpp>

namespace Ame::Rhi
{
    static std::mutex s_Mutex;

    // Intentionally or not, all imgui examples render everything in sRGB space.
    // Whether imgui expected it or not, the display engine then transforms colors to linear space
    // https://stackoverflow.com/a/66401423/4347276
    // We, however, (correctly) render everything in linear space letting the GPU to transform colors to sRGB,
    // so that the display engine then properly shows them.
    //
    // As a result, there is a problem with alpha-blending: imgui performs blending directly in gamma-space, and
    // gamma-to-linear conversion is done by the display engine:
    //
    //   Px_im = GammaToLinear(Src * A + Dst * (1 - A))                     (1)
    //
    // If we only convert imgui colors from sRGB to linear, we will be performing the following (normally)
    // correct blending:
    //
    //   Px_dg = GammaToLinear(Src) * A + GammaToLinear(Dst) * (1 - A)      (2)
    //
    // However in case of imgui, this produces significantly different colors. Consider black background (Dst = 0):
    //
    //   Px_im = GammaToLinear(Src * A)
    //   Px_dg = GammaToLinear(Src) * A
    //
    // We use the following equation that approximates (1):
    //
    //   Px_dg = GammaToLinear(Src * A) + GammaToLinear(Dst) * GammaToLinear(1 - A)  (3)
    //
    // Clearly (3) is not quite the same thing as (1), however it works surprisingly well in practice.
    // Color pickers, in particular look properly.

    // Note that approximate gamma-to-linear conversion pow(gamma, 2.2) produces considerably different colors.
    static constexpr char c_GammaToLinear[] = "((g) < 0.04045 ? (g) / 12.92 : pow(max((g) + 0.055, 0.0) / 1.055, 2.4))";
    static constexpr char c_SrgbaToLinear[] = "c.r = GAMMA_TO_LINEAR(c.r); c.g = GAMMA_TO_LINEAR(c.g); c.b = GAMMA_TO_LINEAR(c.b); c.a = 1.0 - GAMMA_TO_LINEAR(1.0 - c.a);";

    static constexpr char c_Shaders[] = R"(
cbuffer Constants
{
    float4x4 ProjectionMatrix;
}

struct VSInput
{
    float2 pos : ATTRIB0;
    float2 uv  : ATTRIB1;
    float4 col : ATTRIB2;
};

struct PSInput
{
    float4 pos : SV_POSITION;
    float4 col : COLOR;
    float2 uv  : TEXCOORD;
};

#if defined(VERTEX_SHADER)
void main(in VSInput VSIn, out PSInput PSIn)
{
    PSIn.pos = mul(ProjectionMatrix, float4(VSIn.pos.xy, 0.0, 1.0));
    PSIn.col = VSIn.col;
    PSIn.uv  = VSIn.uv;
}
#elif defined(PIXEL_SHADER)
Texture2D    Texture;
SamplerState Texture_sampler;
float4 main(in PSInput PSIn) : SV_Target
{
	float4 col = Texture.Sample(Texture_sampler, PSIn.uv) * PSIn.col;
	col.rgb *= col.a;
	SRGBA_TO_LINEAR(col)
	return col;
}
#endif)";

    //

    ImGuiDiligentRendererImpl::ImGuiDiligentRendererImpl(
        IReferenceCounters*            counters,
        const ImGuiRendererCreateDesc& desc) :
        Base(counters),
        m_ConversionMode(desc.ConversionMode),
        m_MultiThreaded(desc.MultiThreaded)
    {
        desc.RhiDevice->QueryInterface(Dg::IID_RenderDevice, m_RenderDevice.DblPtr<IObject>());
        desc.RhiDevice->QueryInterface(Dg::IID_RenderDevice, m_RenderDevice.DblPtr<IObject>());
        desc.RhiDevice->QueryInterface(Window::IID_Window, m_Window.DblPtr<IObject>());

        Ptr<Dg::ISwapChain> swapChain;
        desc.RhiDevice->QueryInterface(Dg::IID_SwapChain, swapChain.DblPtr<IObject>());

        auto& swapchainDesc = swapChain->GetDesc();

        m_BackBufferFormat  = swapchainDesc.ColorBufferFormat;
        m_DepthBufferFormat = swapchainDesc.DepthBufferFormat;

        // Check base vertex support
        m_BaseVertexSupported = m_RenderDevice->GetAdapterInfo().DrawCommand.CapFlags & Dg::DRAW_COMMAND_CAP_FLAG_BASE_VERTEX;

        // Setup back-end capabilities flags
        if (m_MultiThreaded)
        {
            std::scoped_lock lock(s_Mutex);
            m_Context = ImGui::CreateContext();
            m_Window->InitializeImGui(m_Context);
        }
        else
        {
            m_Context = ImGui::CreateContext();
            m_Window->InitializeImGui(m_Context);
        }

        CreateDeviceObjects();
    }

    ImGuiDiligentRendererImpl::~ImGuiDiligentRendererImpl()
    {
        if (m_MultiThreaded)
        {
            std::scoped_lock lock(s_Mutex);
            ImGui::SetCurrentContext(m_Context);
            m_Window->ShutdownImGui(m_Context);
            ImGui::DestroyContext(m_Context);
        }
        else
        {
            ImGui::SetCurrentContext(m_Context);
            m_Window->ShutdownImGui(m_Context);
            ImGui::DestroyContext(m_Context);
        }

        m_Context = nullptr;
    }

    //

    void ImGuiDiligentRendererImpl::BeginFrame()
    {
        if (!m_PipelineState)
        {
            CreateDeviceObjects();
        }
        if (m_MultiThreaded)
        {
            s_Mutex.lock();
        }

        ImGui::SetCurrentContext(m_Context);

        ImGuiIO& io = ImGui::GetIO();
        if (m_BaseVertexSupported)
        {
            io.BackendFlags |= ImGuiBackendFlags_RendererHasVtxOffset;
        }
        else
        {
            io.BackendFlags &= ~ImGuiBackendFlags_RendererHasVtxOffset;
        }
    }

    void ImGuiDiligentRendererImpl::EndFrame()
    {
        ImGui::SetCurrentContext(nullptr);
        if (m_MultiThreaded)
        {
            s_Mutex.unlock();
        }
    }

    //

    void ImGuiDiligentRendererImpl::SubmitDrawData()
    {
    }

    void ImGuiDiligentRendererImpl::Reset()
    {
        InvalidateDeviceObjects();
    }

    //

    ImFont* ImGuiDiligentRendererImpl::FindFont(
        const String& fontName) const
    {
        auto iter = m_Fonts.find(fontName);
        return iter != m_Fonts.end() ? iter->second : nullptr;
    }

    ImFont* ImGuiDiligentRendererImpl::LoadFont(
        const String&       fontName,
        const ImFontConfig& fontConfig)
    {
#ifdef AME_DEBUG
        if (m_Fonts.contains(fontName))
        {
            Log::Gfx().Warning("Font '{}' already loaded", fontName);
            return nullptr;
        }
#endif

        ImGuiIO& io   = ImGui::GetIO();
        ImFont*  font = io.Fonts->AddFont(&fontConfig);

        m_Fonts.emplace(fontName, font);
        return font;
    }

    ImFont* ImGuiDiligentRendererImpl::LoadCompressedFont(
        const char*         fontName,
        const ImFontConfig& fontConfig)
    {
#ifdef AME_DEBUG
        if (m_Fonts.contains(fontName))
        {
            Log::Gfx().Warning("Font '{}' already loaded", fontName);
            return nullptr;
        }
#endif

        ImGuiIO& io   = ImGui::GetIO();
        ImFont*  font = io.Fonts->AddFontFromMemoryCompressedTTF(fontConfig.FontData, fontConfig.FontDataSize, fontConfig.SizePixels, &fontConfig, fontConfig.GlyphRanges);

        m_Fonts.emplace(fontName, font);
        return font;
    }

    //

    void ImGuiDiligentRendererImpl::InvalidateDeviceObjects()
    {
        m_SRB.Release();
        m_FontTextureView.Release();
        m_TransformBuffer.Release();
        m_PipelineState.Release();
        m_IndexBuffer.Release();
        m_VertexBuffer.Release();
    }

    void ImGuiDiligentRendererImpl::CreateDeviceObjects()
    {
        InvalidateDeviceObjects();

        Dg::ShaderCreateInfo shaderCreateInfo;

        shaderCreateInfo.Source         = c_Shaders;
        shaderCreateInfo.SourceLength   = std::size(c_Shaders);
        shaderCreateInfo.SourceLanguage = Dg::SHADER_SOURCE_LANGUAGE_DEFAULT;

        const auto srgbFramebuffer = Dg::GetTextureFormatAttribs(m_BackBufferFormat).ComponentType == Dg::COMPONENT_TYPE_UNORM_SRGB;
        const auto manualSrgb      = (m_ConversionMode == ImGuiColorConversionMode::Auto && srgbFramebuffer) ||
                                (m_ConversionMode == ImGuiColorConversionMode::SrgbToLinear);
        if (manualSrgb)
        {
            static constexpr Dg::ShaderMacro macros[] = {
                { "GAMMA_TO_LINEAR(g)", c_GammaToLinear },
                { "SRGBA_TO_LINEAR(c)", c_SrgbaToLinear },
            };
            shaderCreateInfo.Macros = { macros, Rhi::Count32(macros) };
        }
        else
        {
            static constexpr Dg::ShaderMacro macros[] = {
                { "SRGBA_TO_LINEAR(c)", "" },
            };
            shaderCreateInfo.Macros = { macros, Rhi::Count32(macros) };
        }

        const auto DeviceType = m_RenderDevice->GetDeviceInfo().Type;

        Ptr<Dg::IShader> vertexShader;
        {
            shaderCreateInfo.Desc = { "ImGui VS", Dg::SHADER_TYPE_VERTEX, true };
            m_RenderDevice->CreateShader(shaderCreateInfo, &vertexShader);
        }

        Ptr<Dg::IShader> pixelShader;
        {
            shaderCreateInfo.Desc = { "ImGui PS", Dg::SHADER_TYPE_PIXEL, true };
            m_RenderDevice->CreateShader(shaderCreateInfo, &pixelShader);
        }

        Dg::GraphicsPipelineStateCreateInfo psoCreateInfo;

        psoCreateInfo.PSODesc.Name = "ImGui PSO";

        auto& graphicsDesc             = psoCreateInfo.GraphicsPipeline;
        graphicsDesc.NumRenderTargets  = 1;
        graphicsDesc.RTVFormats[0]     = m_BackBufferFormat;
        graphicsDesc.DSVFormat         = m_DepthBufferFormat;
        graphicsDesc.PrimitiveTopology = Dg::PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

        graphicsDesc.RasterizerDesc.CullMode      = Dg::CULL_MODE_NONE;
        graphicsDesc.RasterizerDesc.ScissorEnable = true;
        graphicsDesc.DepthStencilDesc.DepthEnable = false;

        psoCreateInfo.pVS = vertexShader;
        psoCreateInfo.pPS = pixelShader;

        auto& rt0       = graphicsDesc.BlendDesc.RenderTargets[0];
        rt0.BlendEnable = true;
        // Use alpha-premultiplied blending, see eq. (3).
        rt0.SrcBlend              = Dg::BLEND_FACTOR_ONE;
        rt0.DestBlend             = Dg::BLEND_FACTOR_INV_SRC_ALPHA;
        rt0.BlendOp               = Dg::BLEND_OPERATION_ADD;
        rt0.SrcBlendAlpha         = Dg::BLEND_FACTOR_ONE;
        rt0.DestBlendAlpha        = Dg::BLEND_FACTOR_INV_SRC_ALPHA;
        rt0.BlendOpAlpha          = Dg::BLEND_OPERATION_ADD;
        rt0.RenderTargetWriteMask = Dg::COLOR_MASK_ALL;

        constexpr Dg::LayoutElement inputs[] //
            {
                { 0, 0, 2, Dg::VT_FLOAT32 },    // pos
                { 1, 0, 2, Dg::VT_FLOAT32 },    // uv
                { 2, 0, 4, Dg::VT_UINT8, true } // col
            };
        graphicsDesc.InputLayout.LayoutElements = inputs;
        graphicsDesc.InputLayout.NumElements    = Rhi::Count32(inputs);

        Dg::ShaderResourceVariableDesc variables[] = {
            { Dg::SHADER_TYPE_PIXEL, "Texture", Dg::SHADER_RESOURCE_VARIABLE_TYPE_DYNAMIC } //
        };
        psoCreateInfo.PSODesc.ResourceLayout.Variables    = variables;
        psoCreateInfo.PSODesc.ResourceLayout.NumVariables = Rhi::Count32(variables);

        Dg::ImmutableSamplerDesc samplers[] = {
            { Dg::SHADER_TYPE_PIXEL, "Texture", Dg::Sam_LinearWrap } //
        };
        psoCreateInfo.PSODesc.ResourceLayout.ImmutableSamplers    = samplers;
        psoCreateInfo.PSODesc.ResourceLayout.NumImmutableSamplers = Rhi::Count32(samplers);

        m_RenderDevice->CreateGraphicsPipelineState(psoCreateInfo, &m_PipelineState);

        Dg::CreateUniformBuffer(m_RenderDevice, sizeof(Math::Matrix4x4), "ImGui Constants", &m_TransformBuffer);
        m_PipelineState->GetStaticVariableByName(Dg::SHADER_TYPE_VERTEX, "Constants")->Set(m_TransformBuffer);
    }

    void ImGuiDiligentRendererImpl::CreateFontsTextures()
    {
        ImGuiIO& io = ImGui::GetIO();

        // If no new fonts were added, there is no need to update the font texture
        if (io.Fonts->IsBuilt())
        {
            return;
        }

        uint8_t* data   = nullptr;
        int      width  = 0;
        int      height = 0;
        io.Fonts->GetTexDataAsRGBA32(&data, &width, &height);

        Dg::TextureDesc fontTextureDesc{
            "ImGui Font Texture",
            Dg::RESOURCE_DIM_TEX_2D,
            static_cast<uint32_t>(width),
            static_cast<uint32_t>(height),
            1,
            Dg::TEX_FORMAT_RGBA8_UNORM
        };
        fontTextureDesc.BindFlags = Dg::BIND_SHADER_RESOURCE;
        fontTextureDesc.Usage     = Dg::USAGE_IMMUTABLE;

        Dg::TextureSubResData mip0Data[] = { { data, sizeof(uint8_t[4]) * fontTextureDesc.Width } };
        Dg::TextureData       initData(mip0Data, Rhi::Count32(mip0Data));

        Ptr<Dg::ITexture> fontTexture;
        m_RenderDevice->CreateTexture(fontTextureDesc, &initData, &fontTexture);
        m_FontTextureView = fontTexture->GetDefaultView(Dg::TEXTURE_VIEW_SHADER_RESOURCE);

        m_SRB.Release();
        m_PipelineState->CreateShaderResourceBinding(&m_SRB, true);

        m_SRB->GetVariableByName(Dg::SHADER_TYPE_PIXEL, "Texture")->Set(fontTexture->GetDefaultView(Dg::TEXTURE_VIEW_SHADER_RESOURCE));

        io.Fonts->TexID = static_cast<ImTextureID>(m_FontTextureView.RawPtr());
    }
} // namespace Ame::Rhi
#endif
