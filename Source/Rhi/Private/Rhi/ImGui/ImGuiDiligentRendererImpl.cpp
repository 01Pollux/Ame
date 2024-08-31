#ifndef AME_NO_IMGUI

#include <mutex>
#include <ranges>

#include <Rhi/ImGui/Fonts/FontAwesome5.hpp>
#include <imgui_internal.h>

#include <Rhi/ImGui/ImGuiDiligentRendererImpl.hpp>
#include <Rhi/ImGui/ImGuiRendererCreateDesc.hpp>

#include <DiligentCore/Graphics/GraphicsTools/interface/CommonlyUsedStates.h>
#include <DiligentCore/Graphics/GraphicsTools/interface/GraphicsUtilities.h>
#include <DiligentCore/Graphics/GraphicsTools/interface/MapHelper.hpp>

#include <Shaders/Rendering/ImGuiRenderShader.hpp>

#include <Rhi/Device/RhiDevice.hpp>
#include <Math/Matrix.hpp>

#include <Log/Logger.hpp>

//

extern unsigned int Arimo_Medium_compressed_data[];
extern unsigned int Arimo_Medium_compressed_size;

extern unsigned int FontAwesomeSolid900_compressed_data[];
extern unsigned int FontAwesomeSolid900_compressed_size;

//

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
    static constexpr char c_SrgbaToLinear[] = "c.r = GAMMA_TO_LINEAR(c.r); c.g = GAMMA_TO_LINEAR(c.g); c.b = "
                                              "GAMMA_TO_LINEAR(c.b); c.a = 1.0 - GAMMA_TO_LINEAR(1.0 - c.a);";

    //

    ImGuiDiligentRendererImpl::ImGuiDiligentRendererImpl(IReferenceCounters*            counters,
                                                         const ImGuiRendererCreateDesc& desc) :
        Base(counters),
        m_MultiThreaded(desc.MultiThreaded), m_VertexBufferSize(desc.InitialVertexBufferSize),
        m_IndexBufferSize(desc.InitialIndexBufferSize), m_ConversionMode(desc.ConversionMode)
    {
        m_RenderDevice  = desc.RhiDevice->GetRenderDevice();
        m_DeviceContext = desc.RhiDevice->GetImmediateContext();
        m_Swapchain     = desc.RhiDevice->GetSwapchain();
        desc.RhiDevice->QueryInterface(Window::IID_ImGuiWindow, m_ImGuiWindow.DblPtr<IObject>());

        auto& swapchainDesc = m_Swapchain->GetDesc();

        m_BackBufferFormat  = swapchainDesc.ColorBufferFormat;
        m_DepthBufferFormat = swapchainDesc.DepthBufferFormat;

        // Check base vertex support
        m_BaseVertexSupported =
            m_RenderDevice->GetAdapterInfo().DrawCommand.CapFlags & Dg::DRAW_COMMAND_CAP_FLAG_BASE_VERTEX;

        if (m_MultiThreaded)
        {
            std::scoped_lock lock(s_Mutex);

            m_Context = ImGui::CreateContext();
            m_ImGuiWindow->InitializeImGui(m_Context);
            m_ImGuiWindow->InstallImGuiCallbacks(m_Context);
        }
        else
        {
            m_Context = ImGui::CreateContext();
            m_ImGuiWindow->InitializeImGui(m_Context);
            m_ImGuiWindow->InstallImGuiCallbacks(m_Context);
        }

        SetDefaultTheme();
        LoadDefaultFonts();
        CreateDeviceObjects();
    }

    ImGuiDiligentRendererImpl::~ImGuiDiligentRendererImpl()
    {
        if (m_MultiThreaded)
        {
            std::scoped_lock lock(s_Mutex);

            ImGui::SetCurrentContext(m_Context);
            m_ImGuiWindow->UninstallImGuiCallbacks(m_Context);
            m_ImGuiWindow->ShutdownImGui(m_Context);
            ImGui::DestroyContext(m_Context);
        }
        else
        {
            ImGui::SetCurrentContext(m_Context);
            m_ImGuiWindow->UninstallImGuiCallbacks(m_Context);
            m_ImGuiWindow->ShutdownImGui(m_Context);
            ImGui::DestroyContext(m_Context);
        }

        m_Context = nullptr;
    }

    //

    void ImGuiDiligentRendererImpl::BeginFrame(Dg::SURFACE_TRANSFORM transform)
    {
        if (!m_PipelineState)
        {
            CreateDeviceObjects();
        }
        if (m_MultiThreaded)
        {
            s_Mutex.lock();
        }

        m_Transform = transform;
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

        CreateFontsTextures();

        m_ImGuiWindow->NewFrameImGui(m_Context);
        ImGui::NewFrame();
    }

    void ImGuiDiligentRendererImpl::EndFrame()
    {
        ImGui::Render();
        SubmitDrawData(ImGui::GetDrawData());

        // Update and Render additional Platform Windows
        if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable) [[likely]]
        {
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault(nullptr, nullptr);
        }

        ImGui::SetCurrentContext(nullptr);
        if (m_MultiThreaded)
        {
            s_Mutex.unlock();
        }
    }

    //

    void ImGuiDiligentRendererImpl::Reset()
    {
        InvalidateDeviceObjects();
    }

    //

    ImFont* ImGuiDiligentRendererImpl::FindFont(const String& fontName) const
    {
        auto iter = m_Fonts.find(fontName);
        return iter != m_Fonts.end() ? iter->second : nullptr;
    }

    ImFont* ImGuiDiligentRendererImpl::LoadFont(const String& fontName, const ImFontConfig& fontConfig)
    {
#ifdef AME_DEBUG
        if (m_Fonts.contains(fontName))
        {
            AME_LOG_WARNING(std::format("Font '{}' already loaded", fontName));
            return nullptr;
        }
#endif

        ImGuiIO& io   = ImGui::GetIO();
        ImFont*  font = io.Fonts->AddFont(&fontConfig);

        m_Fonts.emplace(fontName, font);
        return font;
    }

    ImFont* ImGuiDiligentRendererImpl::LoadCompressedFont(const char* fontName, const ImFontConfig& fontConfig)
    {
#ifdef AME_DEBUG
        if (m_Fonts.contains(fontName))
        {
            AME_LOG_WARNING(std::format("Font '{}' already loaded", fontName));
            return nullptr;
        }
#endif

        ImGuiIO& io   = ImGui::GetIO();
        ImFont*  font = io.Fonts->AddFontFromMemoryCompressedTTF(
            fontConfig.FontData, fontConfig.FontDataSize, fontConfig.SizePixels, &fontConfig, fontConfig.GlyphRanges);

        m_Fonts.emplace(fontName, font);
        return font;
    }

    bool ImGuiDiligentRendererImpl::RenderBackbufferToTexture() const
    {
        return true;
    }

    //

    void ImGuiDiligentRendererImpl::SetDefaultTheme()
    {
        ImGuiStyle& style = m_Context->Style;

        style.Alpha                     = 1.0f;
        style.DisabledAlpha             = 1.0f;
        style.WindowPadding             = ImVec2(4.0f, 12.0f);
        style.WindowBorderSize          = 0.0f;
        style.WindowMinSize             = ImVec2(20.0f, 20.0f);
        style.WindowTitleAlign          = ImVec2(0.5f, 0.5f);
        style.WindowMenuButtonPosition  = ImGuiDir_None;
        style.WindowRounding            = 3.f;
        style.ChildRounding             = 2.f;
        style.ChildBorderSize           = 1.0f;
        style.PopupRounding             = 4.f;
        style.PopupBorderSize           = 1.0f;
        style.FramePadding              = ImVec2(6.0f, 6.0f);
        style.FrameRounding             = 2.f;
        style.FrameBorderSize           = 0.0f;
        style.ItemSpacing               = ImVec2(12.0f, 6.0f);
        style.ItemInnerSpacing          = ImVec2(6.0f, 3.0f);
        style.CellPadding               = ImVec2(12.0f, 6.0f);
        style.IndentSpacing             = 20.0f;
        style.ColumnsMinSpacing         = 6.0f;
        style.ScrollbarSize             = 12.0f;
        style.ScrollbarRounding         = 0.0f;
        style.GrabMinSize               = 12.0f;
        style.GrabRounding              = 0.0f;
        style.TabRounding               = 0.0f;
        style.TabBorderSize             = 0.0f;
        style.TabMinWidthForCloseButton = 0.0f;
        style.ColorButtonPosition       = ImGuiDir_Right;
        style.ButtonTextAlign           = ImVec2(0.5f, 0.5f);
        style.SelectableTextAlign       = ImVec2(0.0f, 0.0f);

        style.Colors[ImGuiCol_Text] = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);
        style.Colors[ImGuiCol_TextDisabled] =
            ImVec4(0.2745098173618317f, 0.3176470696926117f, 0.4509803950786591f, 1.0f);
        style.Colors[ImGuiCol_WindowBg] = ImVec4(0.0784313753247261f, 0.08627451211214066f, 0.1019607856869698f, 1.0f);
        style.Colors[ImGuiCol_ChildBg]  = ImVec4(0.0784313753247261f, 0.08627451211214066f, 0.1019607856869698f, 1.0f);
        style.Colors[ImGuiCol_PopupBg]  = ImVec4(0.0784313753247261f, 0.08627451211214066f, 0.1019607856869698f, 1.0f);
        style.Colors[ImGuiCol_Border]   = ImVec4(0.1568627506494522f, 0.168627455830574f, 0.1921568661928177f, 1.0f);
        style.Colors[ImGuiCol_BorderShadow] =
            ImVec4(0.0784313753247261f, 0.08627451211214066f, 0.1019607856869698f, 1.0f);
        style.Colors[ImGuiCol_FrameBg] = ImVec4(0.1176470592617989f, 0.1333333402872086f, 0.1490196138620377f, 1.0f);
        style.Colors[ImGuiCol_FrameBgHovered] =
            ImVec4(0.1568627506494522f, 0.168627455830574f, 0.1921568661928177f, 1.0f);
        style.Colors[ImGuiCol_FrameBgActive] =
            ImVec4(0.2352941185235977f, 0.2156862765550613f, 0.5960784554481506f, 1.0f);
        style.Colors[ImGuiCol_TitleBg] = ImVec4(0.0470588244497776f, 0.05490196123719215f, 0.07058823853731155f, 1.0f);
        style.Colors[ImGuiCol_TitleBgActive] =
            ImVec4(0.0470588244497776f, 0.05490196123719215f, 0.07058823853731155f, 1.0f);
        style.Colors[ImGuiCol_TitleBgCollapsed] =
            ImVec4(0.0784313753247261f, 0.08627451211214066f, 0.1019607856869698f, 1.0f);
        style.Colors[ImGuiCol_MenuBarBg] = ImVec4(0.09803921729326248f, 0.105882354080677f, 0.1215686276555061f, 1.0f);
        style.Colors[ImGuiCol_ScrollbarBg] =
            ImVec4(0.0470588244497776f, 0.05490196123719215f, 0.07058823853731155f, 1.0f);
        style.Colors[ImGuiCol_ScrollbarGrab] =
            ImVec4(0.1176470592617989f, 0.1333333402872086f, 0.1490196138620377f, 1.0f);
        style.Colors[ImGuiCol_ScrollbarGrabHovered] =
            ImVec4(0.1568627506494522f, 0.168627455830574f, 0.1921568661928177f, 1.0f);
        style.Colors[ImGuiCol_ScrollbarGrabActive] =
            ImVec4(0.1176470592617989f, 0.1333333402872086f, 0.1490196138620377f, 1.0f);
        style.Colors[ImGuiCol_CheckMark]        = ImVec4(0.4980392158031464f, 0.5137255191802979f, 1.0f, 1.0f);
        style.Colors[ImGuiCol_SliderGrab]       = ImVec4(0.4980392158031464f, 0.5137255191802979f, 1.0f, 1.0f);
        style.Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.5372549295425415f, 0.5529412031173706f, 1.0f, 1.0f);
        style.Colors[ImGuiCol_Button] = ImVec4(0.1176470592617989f, 0.1333333402872086f, 0.1490196138620377f, 1.0f);
        style.Colors[ImGuiCol_ButtonHovered] =
            ImVec4(0.196078434586525f, 0.1764705926179886f, 0.5450980663299561f, 1.0f);
        style.Colors[ImGuiCol_ButtonActive] =
            ImVec4(0.2352941185235977f, 0.2156862765550613f, 0.5960784554481506f, 1.0f);
        style.Colors[ImGuiCol_Header] = ImVec4(0.1176470592617989f, 0.1333333402872086f, 0.1490196138620377f, 1.0f);
        style.Colors[ImGuiCol_HeaderHovered] =
            ImVec4(0.196078434586525f, 0.1764705926179886f, 0.5450980663299561f, 1.0f);
        style.Colors[ImGuiCol_HeaderActive] =
            ImVec4(0.2352941185235977f, 0.2156862765550613f, 0.5960784554481506f, 1.0f);
        style.Colors[ImGuiCol_Separator] = ImVec4(0.1568627506494522f, 0.1843137294054031f, 0.250980406999588f, 1.0f);
        style.Colors[ImGuiCol_SeparatorHovered] =
            ImVec4(0.1568627506494522f, 0.1843137294054031f, 0.250980406999588f, 1.0f);
        style.Colors[ImGuiCol_SeparatorActive] =
            ImVec4(0.1568627506494522f, 0.1843137294054031f, 0.250980406999588f, 1.0f);
        style.Colors[ImGuiCol_ResizeGrip] = ImVec4(0.1176470592617989f, 0.1333333402872086f, 0.1490196138620377f, 1.0f);
        style.Colors[ImGuiCol_ResizeGripHovered] =
            ImVec4(0.196078434586525f, 0.1764705926179886f, 0.5450980663299561f, 1.0f);
        style.Colors[ImGuiCol_ResizeGripActive] =
            ImVec4(0.2352941185235977f, 0.2156862765550613f, 0.5960784554481506f, 1.0f);
        style.Colors[ImGuiCol_Tab] = ImVec4(0.0470588244497776f, 0.05490196123719215f, 0.07058823853731155f, 1.0f);
        style.Colors[ImGuiCol_TabHovered] = ImVec4(0.1176470592617989f, 0.1333333402872086f, 0.1490196138620377f, 1.0f);
        style.Colors[ImGuiCol_TabSelected] =
            ImVec4(0.09803921729326248f, 0.105882354080677f, 0.1215686276555061f, 1.0f);
        style.Colors[ImGuiCol_TabDimmed] =
            ImVec4(0.0470588244497776f, 0.05490196123719215f, 0.07058823853731155f, 1.0f);
        style.Colors[ImGuiCol_TabDimmedSelected] =
            ImVec4(0.0784313753247261f, 0.08627451211214066f, 0.1019607856869698f, 1.0f);
        style.Colors[ImGuiCol_PlotLines] = ImVec4(0.5215686559677124f, 0.6000000238418579f, 0.7019608020782471f, 1.0f);
        style.Colors[ImGuiCol_PlotLinesHovered] =
            ImVec4(0.03921568766236305f, 0.9803921580314636f, 0.9803921580314636f, 1.0f);
        style.Colors[ImGuiCol_PlotHistogram] = ImVec4(1.0f, 0.2901960909366608f, 0.5960784554481506f, 1.0f);
        style.Colors[ImGuiCol_PlotHistogramHovered] =
            ImVec4(0.9960784316062927f, 0.4745098054409027f, 0.6980392336845398f, 1.0f);
        style.Colors[ImGuiCol_TableHeaderBg] =
            ImVec4(0.0470588244497776f, 0.05490196123719215f, 0.07058823853731155f, 1.0f);
        style.Colors[ImGuiCol_TableBorderStrong] =
            ImVec4(0.0470588244497776f, 0.05490196123719215f, 0.07058823853731155f, 1.0f);
        style.Colors[ImGuiCol_TableBorderLight] = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
        style.Colors[ImGuiCol_TableRowBg] = ImVec4(0.1176470592617989f, 0.1333333402872086f, 0.1490196138620377f, 1.0f);
        style.Colors[ImGuiCol_TableRowBgAlt] =
            ImVec4(0.09803921729326248f, 0.105882354080677f, 0.1215686276555061f, 1.0f);
        style.Colors[ImGuiCol_TextSelectedBg] =
            ImVec4(0.2352941185235977f, 0.2156862765550613f, 0.5960784554481506f, 1.0f);
        style.Colors[ImGuiCol_DragDropTarget]        = ImVec4(0.4980392158031464f, 0.5137255191802979f, 1.0f, 1.0f);
        style.Colors[ImGuiCol_NavHighlight]          = ImVec4(0.4980392158031464f, 0.5137255191802979f, 1.0f, 1.0f);
        style.Colors[ImGuiCol_NavWindowingHighlight] = ImVec4(0.4980392158031464f, 0.5137255191802979f, 1.0f, 1.0f);
        style.Colors[ImGuiCol_NavWindowingDimBg] =
            ImVec4(0.196078434586525f, 0.1764705926179886f, 0.5450980663299561f, 0.501960813999176f);
        style.Colors[ImGuiCol_ModalWindowDimBg] =
            ImVec4(0.196078434586525f, 0.1764705926179886f, 0.5450980663299561f, 0.501960813999176f);
    }

    void ImGuiDiligentRendererImpl::LoadDefaultFonts()
    {
        auto loadFont = []
        {
            ImGuiIO& io = ImGui::GetIO();

            // First is the interface's default font
            {
                ImFontConfig fontCfg;
                fontCfg.OversampleH = 2;

                io.Fonts->AddFontFromMemoryCompressedTTF(Arimo_Medium_compressed_data, Arimo_Medium_compressed_size,
                                                         15.f, &fontCfg,
                                                         ImGui::GetIO().Fonts->GetGlyphRangesCyrillic());
            }

            // Second is the interface's default icons
            {
                ImFontConfig fontCfg;
                fontCfg.MergeMode = true;

                static const ImWchar iconRanges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };

                io.Fonts->AddFontFromMemoryCompressedTTF(FontAwesomeSolid900_compressed_data,
                                                         FontAwesomeSolid900_compressed_size, 13.f, &fontCfg,
                                                         iconRanges);
            }
        };

        static std::once_flag s_Loaded;
        std::call_once(s_Loaded, loadFont);
    }

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

        const auto srgbFramebuffer =
            Dg::GetTextureFormatAttribs(m_BackBufferFormat).ComponentType == Dg::COMPONENT_TYPE_UNORM_SRGB;
        const auto manualSrgb = (m_ConversionMode == ImGuiColorConversionMode::Auto && srgbFramebuffer) ||
                                (m_ConversionMode == ImGuiColorConversionMode::SrgbToLinear);

        Ptr<Dg::IShader> vertexShader;
        {
            ImGuiRenderVertexShader shaderDesc;
            m_RenderDevice->CreateShader(shaderDesc, &vertexShader);
        }

        Ptr<Dg::IShader> pixelShader;
        {
            ImGuiRenderPixelShader shaderDesc;
            if (manualSrgb)
            {
                shaderDesc.AddMacro("GAMMA_TO_LINEAR(g)", c_GammaToLinear);
                shaderDesc.AddMacro("SRGBA_TO_LINEAR(c)", c_SrgbaToLinear);
            }
            m_RenderDevice->CreateShader(shaderDesc, &pixelShader);
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

        Dg::TextureDesc fontTextureDesc{ "ImGui Font Texture",
                                         Dg::RESOURCE_DIM_TEX_2D,
                                         static_cast<uint32_t>(width),
                                         static_cast<uint32_t>(height),
                                         1,
                                         Dg::TEX_FORMAT_RGBA8_UNORM };
        fontTextureDesc.BindFlags = Dg::BIND_SHADER_RESOURCE;
        fontTextureDesc.Usage     = Dg::USAGE_IMMUTABLE;

        Dg::TextureSubResData mip0Data[] = { { data, sizeof(uint8_t[4]) * fontTextureDesc.Width } };
        Dg::TextureData       initData(mip0Data, Rhi::Count32(mip0Data));

        Ptr<Dg::ITexture> fontTexture;
        m_RenderDevice->CreateTexture(fontTextureDesc, &initData, &fontTexture);
        m_FontTextureView = fontTexture->GetDefaultView(Dg::TEXTURE_VIEW_SHADER_RESOURCE);

        m_SRB.Release();
        m_PipelineState->CreateShaderResourceBinding(&m_SRB, true);
        m_TextureVariable = m_SRB->GetVariableByName(Dg::SHADER_TYPE_PIXEL, "Texture");

        io.Fonts->TexID = static_cast<ImTextureID>(m_FontTextureView.RawPtr());
    }

    //

    static Math::Vector4 TransformClipRect(Dg::SURFACE_TRANSFORM transform, const ImVec2& displaySize,
                                           const Math::Vector4& rect)
    {
        switch (transform)
        {
        case Dg::SURFACE_TRANSFORM_IDENTITY:
            return rect;

        case Dg::SURFACE_TRANSFORM_ROTATE_90:
        {
            // The image content is rotated 90 degrees clockwise. The origin is in the left-top corner.
            //
            //                                                             DsplSz.y
            //                a.x                                            -a.y     a.y     Old origin
            //              0---->|                                       0------->|<------| /
            //           0__|_____|____________________                0__|________|_______|/
            //            | |     '                    |                | |        '       |
            //        a.y | |     '                    |            a.x | |        '       |
            //           _V_|_ _ _a____b               |               _V_|_ _d'___a'      |
            //            A |     |    |               |                  |   |    |       |
            //  DsplSz.y  | |     |____|               |                  |   |____|       |
            //    -a.y    | |     d    c               |                  |   c'   b'      |
            //           _|_|__________________________|                  |                |
            //              A                                             |                |
            //              |-----> Y'                                    |                |
            //         New Origin                                         |________________|
            //
            Math::Vector2 a{ rect.x(), rect.y() };
            Math::Vector2 c{ rect.z(), rect.w() };
            return Math::Vector4 //
                {
                    displaySize.y - c.y(), // min_x = c'.x
                    a.x(),                 // min_y = a'.y
                    displaySize.y - a.y(), // max_x = a'.x
                    c.x()                  // max_y = c'.y
                };
        }

        case Dg::SURFACE_TRANSFORM_ROTATE_180:
        {
            // The image content is rotated 180 degrees clockwise. The origin is in the left-top corner.
            //
            //                a.x                                               DsplSz.x - a.x
            //              0---->|                                         0------------------>|
            //           0__|_____|____________________                 0_ _|___________________|______
            //            | |     '                    |                  | |                   '      |
            //        a.y | |     '                    |        DsplSz.y  | |              c'___d'     |
            //           _V_|_ _ _a____b               |          -a.y    | |              |    |      |
            //              |     |    |               |                 _V_|_ _ _ _ _ _ _ |____|      |
            //              |     |____|               |                    |              b'   a'     |
            //              |     d    c               |                    |                          |
            //              |__________________________|                    |__________________________|
            //                                         A                                               A
            //                                         |                                               |
            //                                     New Origin                                      Old Origin
            Math::Vector2 a{ rect.x(), rect.y() };
            Math::Vector2 c{ rect.z(), rect.w() };
            return Math::Vector4 //
                {
                    displaySize.x - c.x(), // min_x = c'.x
                    displaySize.y - c.y(), // min_y = c'.y
                    displaySize.x - a.x(), // max_x = a'.x
                    displaySize.y - a.y()  // max_y = a'.y
                };
        }

        case Dg::SURFACE_TRANSFORM_ROTATE_270:
        {
            // The image content is rotated 270 degrees clockwise. The origin is in the left-top corner.
            //
            //              0  a.x     DsplSz.x-a.x   New Origin              a.y
            //              |---->|<-------------------|                    0----->|
            //          0_ _|_____|____________________V                 0 _|______|_________
            //            | |     '                    |                  | |      '         |
            //            | |     '                    |                  | |      '         |
            //        a.y_V_|_ _ _a____b               |        DsplSz.x  | |      '         |
            //              |     |    |               |          -a.x    | |      '         |
            //              |     |____|               |                  | |      b'___c'   |
            //              |     d    c               |                  | |      |    |    |
            //  DsplSz.y _ _|__________________________|                 _V_|_ _ _ |____|    |
            //                                                              |      a'   d'   |
            //                                                              |                |
            //                                                              |________________|
            //                                                              A
            //                                                              |
            //                                                            Old origin
            Math::Vector2 a{ rect.x(), rect.y() };
            Math::Vector2 c{ rect.z(), rect.w() };
            return Math::Vector4 //
                {
                    a.y(),                 // min_x = a'.x
                    displaySize.x - c.x(), // min_y = c'.y
                    c.y(),                 // max_x = c'.x
                    displaySize.x - a.x()  // max_y = a'.y
                };
        }

        case Dg::SURFACE_TRANSFORM_OPTIMAL:
            AME_LOG_WARNING("SURFACE_TRANSFORM_OPTIMAL is only valid as parameter during swap chain initialization.");
            return rect;

        case Dg::SURFACE_TRANSFORM_HORIZONTAL_MIRROR:
        case Dg::SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_90:
        case Dg::SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_180:
        case Dg::SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_270:
            AME_LOG_WARNING("Mirror transforms are not supported");
            return rect;

        default:
            std::unreachable();
        }
    }

    void ImGuiDiligentRendererImpl::SubmitDrawData(ImDrawData* drawData)
    {
        // Avoid rendering when minimized
        if (drawData->DisplaySize.x <= 0.0f || drawData->DisplaySize.y <= 0.0f || drawData->CmdListsCount == 0)
        {
            return;
        }

        auto reserveBuffer = [this](auto& buffer, const char* name, auto& bufferSize, auto totalCount, auto structSize,
                                    Dg::BIND_FLAGS flags)
        {
            auto totalSize = totalCount * structSize;
            if (!buffer || bufferSize < totalSize)
            {
                buffer.Release();
                while (bufferSize < totalSize)
                {
                    bufferSize *= 2;
                }

                Dg::BufferDesc bufferDesc{ name, bufferSize, flags, Dg::USAGE_DYNAMIC, Dg::CPU_ACCESS_WRITE };
                m_RenderDevice->CreateBuffer(bufferDesc, nullptr, &buffer);
            }
        };
        reserveBuffer(m_VertexBuffer, "ImGui Vertex Buffer", m_VertexBufferSize, drawData->TotalVtxCount,
                      sizeof(ImDrawVert), Dg::BIND_VERTEX_BUFFER);
        reserveBuffer(m_IndexBuffer, "ImGui Index Buffer", m_IndexBufferSize, drawData->TotalIdxCount,
                      sizeof(ImDrawIdx), Dg::BIND_INDEX_BUFFER);

        {
            Dg::MapHelper<ImDrawVert> vertices(m_DeviceContext, m_VertexBuffer, Dg::MAP_WRITE, Dg::MAP_FLAG_DISCARD);
            Dg::MapHelper<ImDrawIdx>  indices(m_DeviceContext, m_IndexBuffer, Dg::MAP_WRITE, Dg::MAP_FLAG_DISCARD);

            ImDrawVert* vertexDst = vertices;
            ImDrawIdx*  indexDst  = indices;
            for (int id : std::views::iota(0, drawData->CmdListsCount))
            {
                const ImDrawList* cmdList = drawData->CmdLists[id];

                memcpy(vertexDst, cmdList->VtxBuffer.Data, cmdList->VtxBuffer.Size * sizeof(ImDrawVert));
                memcpy(indexDst, cmdList->IdxBuffer.Data, cmdList->IdxBuffer.Size * sizeof(ImDrawIdx));

                vertexDst += cmdList->VtxBuffer.Size;
                indexDst += cmdList->IdxBuffer.Size;
            }
        }

        // Setup orthographic projection matrix into our constant buffer
        // Our visible imgui space lies from drawData->DisplayPos (top left) to
        // drawData->DisplayPos+data_data->DisplaySize (bottom right). DisplayPos is (0,0) for single viewport apps.
        {
            // DisplaySize always refers to the logical dimensions that account for pre-transform, hence
            // the aspect ratio will be correct after applying appropriate rotation.
            float L = drawData->DisplayPos.x;
            float R = drawData->DisplayPos.x + drawData->DisplaySize.x;
            float T = drawData->DisplayPos.y;
            float B = drawData->DisplayPos.y + drawData->DisplaySize.y;

            // clang-format off
            Math::Matrix4x4 projection{
                { 2.0f / (R - L),                  0.0f,   0.0f,   0.0f },
                { 0.0f,                  2.0f / (T - B),   0.0f,   0.0f },
                { 0.0f,                            0.0f,   0.5f,   0.0f },
                { (R + L) / (L - R),  (T + B) / (B - T),   0.5f,   1.0f } };

            // clang-format on

            // Bake pre-transform into projection
            switch (m_Transform)
            {
            case Dg::SURFACE_TRANSFORM_IDENTITY:
                // Nothing to do
                break;

            case Dg::SURFACE_TRANSFORM_ROTATE_90:
                // The image content is rotated 90 degrees clockwise.
                projection *= Math::Matrix4x4::RotationZ(-std::numbers::pi_v<float> * 0.5f);
                break;

            case Dg::SURFACE_TRANSFORM_ROTATE_180:
                // The image content is rotated 180 degrees clockwise.
                projection *= Math::Matrix4x4::RotationZ(-std::numbers::pi_v<float>);
                break;

            case Dg::SURFACE_TRANSFORM_ROTATE_270:
                // The image content is rotated 270 degrees clockwise.
                projection *= Math::Matrix4x4::RotationZ(-std::numbers::pi_v<float> * 1.5f);
                break;

            case Dg::SURFACE_TRANSFORM_OPTIMAL:
                AME_LOG_WARNING(
                    "SURFACE_TRANSFORM_OPTIMAL is only valid as parameter during swap chain initialization.");
                break;

            case Dg::SURFACE_TRANSFORM_HORIZONTAL_MIRROR:
            case Dg::SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_90:
            case Dg::SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_180:
            case Dg::SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_270:
                AME_LOG_WARNING("Mirror transforms are not supported");
                break;

            default:
                std::unreachable();
            }

            Dg::MapHelper<Math::Matrix4x4> transform(
                m_DeviceContext, m_TransformBuffer, Dg::MAP_WRITE, Dg::MAP_FLAG_DISCARD);
            *transform = projection;
        }

        auto setupRenderState = [&]() //
        {
            Dg::ITextureView* renderTarget = m_Swapchain->GetCurrentBackBufferRTV();
            Dg::ITextureView* depthStencil = m_Swapchain->GetDepthBufferDSV();

            m_DeviceContext->SetRenderTargets(
                1, &renderTarget, depthStencil, Dg::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);

            // Setup shader and vertex buffers
            Dg::IBuffer* vertices[] = { m_VertexBuffer };
            m_DeviceContext->SetVertexBuffers(0, 1, vertices, nullptr, Dg::RESOURCE_STATE_TRANSITION_MODE_TRANSITION,
                                              Dg::SET_VERTEX_BUFFERS_FLAG_RESET);
            m_DeviceContext->SetIndexBuffer(m_IndexBuffer, 0, Dg::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
            m_DeviceContext->SetPipelineState(m_PipelineState);

            const float blend_factor[4] = { 0.f, 0.f, 0.f, 0.f };
            m_DeviceContext->SetBlendFactors(blend_factor);

            Dg::Viewport viewport{ 0.0f, 0.0f, drawData->DisplaySize.x, drawData->DisplaySize.y, 0.0f, 1.0f };
            m_DeviceContext->SetViewports(1, &viewport, 0, 0);
        };

        setupRenderState();

        // Render command lists
        // (Because we merged all buffers into a single one, we maintain our own offset into them)
        uint32_t globalIdxOffset = 0;
        uint32_t globalVtxOffset = 0;

        Dg::ITextureView* lastTextureView = nullptr;
        for (int id : std::views::iota(0, drawData->CmdListsCount))
        {
            const ImDrawList* cmdList = drawData->CmdLists[id];
            for (auto cmdId : std::views::iota(0, cmdList->CmdBuffer.Size))
            {
                const ImDrawCmd* cmd = &cmdList->CmdBuffer[cmdId];
                if (cmd->UserCallback != NULL)
                {
                    // User callback, registered via ImDrawList::AddCallback()
                    // (ImDrawCallback_ResetRenderState is a special callback value used by the user to request the
                    // renderer to reset render state.)
                    if (cmd->UserCallback == ImDrawCallback_ResetRenderState)
                    {
                        setupRenderState();
                    }
                    else
                    {
                        cmd->UserCallback(cmdList, cmd);
                    }
                }
                else
                {
                    if (cmd->ElemCount == 0)
                    {
                        continue;
                    }

                    // Apply scissor/clipping rectangle
                    Math::Vector4 clipRect //
                        {
                            (cmd->ClipRect.x - drawData->DisplayPos.x) * drawData->FramebufferScale.x,
                            (cmd->ClipRect.y - drawData->DisplayPos.y) * drawData->FramebufferScale.y,
                            (cmd->ClipRect.z - drawData->DisplayPos.x) * drawData->FramebufferScale.x,
                            (cmd->ClipRect.w - drawData->DisplayPos.y) * drawData->FramebufferScale.y //
                        };
                    // Apply pretransform
                    clipRect = TransformClipRect(m_Transform, drawData->DisplaySize, clipRect);

                    Dg::Rect scissor //
                        {
                            static_cast<int>(clipRect.x()), static_cast<int>(clipRect.y()),
                            static_cast<int>(clipRect.z()),
                            static_cast<int>(clipRect.w()) //
                        };
                    scissor.left   = std::max(scissor.left, 0);
                    scissor.top    = std::max(scissor.top, 0);
                    scissor.right  = std::min(scissor.right, static_cast<int>(drawData->DisplaySize.x));
                    scissor.bottom = std::min(scissor.bottom, static_cast<int>(drawData->DisplaySize.y));
                    if (!scissor.IsValid())
                    {
                        continue;
                    }
                    m_DeviceContext->SetScissorRects(1, &scissor, 0, 0);

                    // Bind texture
                    auto textureView = static_cast<Dg::ITextureView*>(cmd->TextureId);
                    AME_LOG_ASSERT(textureView, "Texture view is null");
                    if (textureView != lastTextureView)
                    {
                        lastTextureView = textureView;
                        m_TextureVariable->Set(textureView);
                        m_DeviceContext->CommitShaderResources(m_SRB, Dg::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
                    }

                    Dg::DrawIndexedAttribs drawAtributes{ cmd->ElemCount,
                                                          sizeof(ImDrawIdx) == sizeof(uint16_t) ? Dg::VT_UINT16
                                                                                                : Dg::VT_UINT32,
                                                          Dg::DRAW_FLAG_VERIFY_STATES };
                    drawAtributes.FirstIndexLocation = cmd->IdxOffset + globalIdxOffset;
                    if (m_BaseVertexSupported)
                    {
                        drawAtributes.BaseVertex = cmd->VtxOffset + globalVtxOffset;
                    }
                    else
                    {
                        Dg::IBuffer* vertices[] = { m_VertexBuffer };
                        uint64_t     offsets[]  = { sizeof(ImDrawVert) * (size_t{ cmd->VtxOffset } + globalVtxOffset) };
                        m_DeviceContext->SetVertexBuffers(0, 1, vertices, offsets,
                                                          Dg::RESOURCE_STATE_TRANSITION_MODE_TRANSITION,
                                                          Dg::SET_VERTEX_BUFFERS_FLAG_NONE);
                    }
                    m_DeviceContext->DrawIndexed(drawAtributes);
                }
            }
            globalIdxOffset += cmdList->IdxBuffer.Size;
            globalVtxOffset += cmdList->VtxBuffer.Size;
        }
    }
} // namespace Ame::Rhi
#endif
