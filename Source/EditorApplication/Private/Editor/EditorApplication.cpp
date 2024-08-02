#include <Editor/EditorApplication.hpp>
#include <Engine/Engine.hpp>

#include <Module/Rhi/RhiModule.hpp>
#include <Module/Graphics/GraphicsModule.hpp>
#include <Module/Editor/EditorModule.hpp>

//

#include <Module/Graphics/RendererSubmodule.hpp>
#include <EcsComponent/Renderables/3D/ModelLoader.hpp>
#include <Shading/Technique.hpp>
#include <Shading/Material.hpp>
#include <Core/Enum.hpp>

namespace Ame
{
    EditorApplication::EditorApplication(
        const EditorApplicationConfig& config) :
        Base(config.Application)
    {
        auto& moduleRegistry = GetEngine().GetRegistry();

        EditorModule::Dependencies deps{
            moduleRegistry.GetModule<RhiModule>(IID_RhiModule),
            moduleRegistry.GetModule<GraphicsModule>(IID_GraphicsModule)
        };
        moduleRegistry.RegisterModule<EditorModule>(deps, config.EditorConfig);
    }

    void EditorApplication::OnLoad()
    {
        Base::OnLoad();

        //

        Ptr<Dg::IRenderDevice> renderDevice;
        GetEngine().GetRegistry().GetModule(IID_RhiModule)->QueryInterface(Dg::IID_RenderDevice, renderDevice.DblPtr<IObject>());

        Ptr<Dg::ISwapChain> swapChain;
        GetEngine().GetRegistry().GetModule(IID_RhiModule)->QueryInterface(Dg::IID_SwapChain, swapChain.DblPtr<IObject>());

        Ptr<Dg::IDeviceContext> deviceContext;
        GetEngine().GetRegistry().GetModule(IID_RhiModule)->QueryInterface(Dg::IID_DeviceContext, deviceContext.DblPtr<IObject>());

        deviceContext->Flush();
        deviceContext->Flush();
        Ptr mdl(Ecs::MeshModelLoader::LoadModel({ .RenderDevice = renderDevice, .ModelPath = "Shared/Assets/Models/Sponza/sponza.obj" }));
        deviceContext->Flush();

        //

        Rhi::MaterialRenderState renderState;

        renderState.Links.ShaderSources[Dg::SHADER_TYPE_VERTEX].ShaderCode = R"(
struct vs_input {
    VS_INPUT_LAYOUT
};

struct vs_output {
	VS_OUTPUT_LAYOUT
};

void do_main(in vs_input input, out vs_output output) {
	output.screen_position = float4(input.position, 1.0);
	output.world_position = input.position;
	output.world_normal = input.normal;
	output.world_tangent = input.tangent;
	output.tex_coord = input.tex_coord;
};

void main(in vs_input input, out vs_output output) {
	do_main(input, output);
};
)";

        renderState.Links.ShaderSources[Dg::SHADER_TYPE_PIXEL].ShaderCode = R"(
struct vs_output {
	VS_OUTPUT_LAYOUT
};

struct ps_input {
    float4 color: SV_TARGET;
};

void do_main(in vs_output input, out ps_input output) {
	output.color = float4(1.0, 0.0, 0.0, 1.0);
};

void main(in vs_output input, out ps_input output) {
	do_main(input, output);
};
)";
        renderState.RenderTargets.emplace_back(swapChain->GetCurrentBackBufferRTV()->GetDesc().Format);

        Ptr technique(Rhi::MaterialTechnique::Create(renderDevice, renderState));

        //

        Rhi::MaterialCreateDesc materialDesc;

        Ptr material(Rhi::Material::Create(renderDevice, materialDesc));

        //

        Ptr<RendererSubmodule> rendererSubmodule;
        GetEngine().GetRegistry().GetModule(IID_GraphicsModule)->QueryInterface(IID_RendererSubmodule, rendererSubmodule.DblPtr<IObject>());

        rendererSubmodule->OnRenderUpdate(
            [mdl, deviceContext, technique, material, swapChain]()
            {
                auto rtv = swapChain->GetCurrentBackBufferRTV();
                deviceContext->SetRenderTargets(1, &rtv, nullptr, Dg::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);

                // Clear the back buffer
                const float ClearColor[] = { 0.032f, 0.032f, 0.032f, 1.0f };
                deviceContext->ClearRenderTarget(rtv, ClearColor, Dg::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);

                using namespace EnumBitOperators;

                Rhi::MaterialVertexInputFlags vertexInputFlags =
                    Rhi::MaterialVertexInputFlags::Position |
                    Rhi::MaterialVertexInputFlags::Normal |
                    Rhi::MaterialVertexInputFlags::TexCoord |
                    Rhi::MaterialVertexInputFlags::Tangent;
                auto pso = technique->GetPipelineState(vertexInputFlags, Dg::PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, material);

                deviceContext->SetPipelineState(pso);

                std::array<Dg ::IBuffer*, std::to_underlying(Rhi::MaterialVertexInputTypes::Count)> vertexBuffers{};

                vertexBuffers[std::to_underlying(Rhi::MaterialVertexInputTypes::Position)] = mdl->GetPositionBuffer();
                vertexBuffers[std::to_underlying(Rhi::MaterialVertexInputTypes::Normal)]   = mdl->GetNormalBuffer();
                vertexBuffers[std::to_underlying(Rhi::MaterialVertexInputTypes::TexCoord)] = mdl->GetTexCoordBuffer();
                vertexBuffers[std::to_underlying(Rhi::MaterialVertexInputTypes::Tangent)]  = mdl->GetTangentBuffer();

                deviceContext->SetVertexBuffers(0, Rhi::Count32(vertexBuffers), vertexBuffers.data(), nullptr, Dg::RESOURCE_STATE_TRANSITION_MODE_TRANSITION, Dg::SET_VERTEX_BUFFERS_FLAG_RESET);
                deviceContext->SetIndexBuffer(mdl->GetIndexBuffer(), 0, Dg::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);

                for (auto& submesh : mdl->GetSubMeshes())
                {
                    Dg::DrawIndexedAttribs drawAttribs(submesh.IndexCount , mdl->GetIndexType(), Dg::DRAW_FLAG_VERIFY_ALL, 1, submesh.IndexOffset, submesh.PositionOffset);
                    deviceContext->DrawIndexed(drawAttribs);
                }
            });
    }

    void EditorApplication::OnInitialize()
    {
        Base::OnInitialize();
    }

    void EditorApplication::OnShutdown()
    {
        Base::OnShutdown();
    }

    void EditorApplication::OnUnload()
    {
        Base::OnUnload();
    }
} // namespace Ame
