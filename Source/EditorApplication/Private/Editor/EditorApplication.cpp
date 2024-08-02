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
#include <Math/Common.hpp>
#include <DiligentCore/Graphics/GraphicsTools/interface/MapHelper.hpp>

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
struct EntityInstance
{
	uint position_offset;
	uint normal_offset;
	uint tex_coord_offset;
	uint tangent_offset;
};

StructuredBuffer<float3> g_Positions;
StructuredBuffer<float3> g_Normals;
StructuredBuffer<float2> g_TexCoords;
StructuredBuffer<float3> g_Tangents;
StructuredBuffer<EntityInstance> g_EntityInstances;

struct vs_input {
    VS_INPUT_LAYOUT
};

struct vs_output {
	VS_OUTPUT_LAYOUT
};

void do_main(in vs_input vsIn, out vs_output vsOut) {
	EntityInstance instance = g_EntityInstances[vsIn.instance_id];

    float3 position = g_Positions[vsIn.vertex_id + instance.position_offset];
	float3 normal = g_Normals[vsIn.vertex_id + instance.normal_offset];
	float2 tex_coord = g_TexCoords[vsIn.vertex_id + instance.tex_coord_offset];
	float3 tangent = g_Tangents[vsIn.vertex_id + instance.tangent_offset];

	vsOut.screen_position = float4(position, 1.0);
	vsOut.world_position = position;
	vsOut.world_normal = normal;
	vsOut.world_tangent = tangent;
	vsOut.tex_coord = tex_coord;
};

void main(in vs_input vsIn, out vs_output vsOut) {
	do_main(vsIn, vsOut);
};
)";

        renderState.Links.ShaderSources[Dg::SHADER_TYPE_PIXEL].ShaderCode = R"(
struct ps_input {
	VS_OUTPUT_LAYOUT
};

struct ps_output {
    float4 color: SV_TARGET;
};

void do_main(in ps_input psIn, out ps_output psOut) {
	psOut.color = float4(1.0, 0.0, 0.0, 1.0);
};

void main(in ps_input psIn, out ps_output psOut) {
	do_main(psIn, psOut);
};
)";
        Ptr<Dg::IShaderResourceBinding>     entityResourceBinding;
        Ptr<Dg::IPipelineResourceSignature> entityResourceSignature;
        {

            constexpr Dg::PipelineResourceDesc resources[]{
                { Dg::SHADER_TYPE::SHADER_TYPE_ALL_GRAPHICS, "g_Positions", Dg::SHADER_RESOURCE_TYPE_BUFFER_SRV },
                { Dg::SHADER_TYPE::SHADER_TYPE_ALL_GRAPHICS, "g_Normals", Dg::SHADER_RESOURCE_TYPE_BUFFER_SRV },
                { Dg::SHADER_TYPE::SHADER_TYPE_ALL_GRAPHICS, "g_TexCoords", Dg::SHADER_RESOURCE_TYPE_BUFFER_SRV },
                { Dg::SHADER_TYPE::SHADER_TYPE_ALL_GRAPHICS, "g_Tangents", Dg::SHADER_RESOURCE_TYPE_BUFFER_SRV },
                { Dg::SHADER_TYPE::SHADER_TYPE_ALL_GRAPHICS, "g_EntityInstances", Dg::SHADER_RESOURCE_TYPE_BUFFER_SRV }
            };

            Dg::PipelineResourceSignatureDesc desc{
                .Resources    = resources,
                .NumResources = Rhi::Count32(resources)
            };

            renderDevice->CreatePipelineResourceSignature(desc, &entityResourceSignature);
            entityResourceSignature->CreateShaderResourceBinding(&entityResourceBinding);
        }

        renderState.Signatures.emplace_back(entityResourceSignature);
        renderState.RenderTargets.emplace_back(swapChain->GetCurrentBackBufferRTV()->GetDesc().Format);

        Ptr technique(Rhi::MaterialTechnique::Create(renderDevice, renderState));

        //

        Rhi::MaterialCreateDesc materialDesc;

        Ptr material(Rhi::Material::Create(renderDevice, materialDesc));

        //

        struct EntityInstance
        {
            uint32_t position_offset;
            uint32_t normal_offset;
            uint32_t tex_coord_offset;
            uint32_t tangent_offset;
        };
        size_t alignedEntityInstanceSize = Math::AlignUp(sizeof(EntityInstance), renderDevice->GetAdapterInfo().Buffer.StructuredBufferOffsetAlignment);

        Ptr<Dg::IBuffer> entityInstanceBuffer;
        Dg::BufferDesc   bufferDesc{
            "EntityInstanceBuffer",
            alignedEntityInstanceSize * 128,
            Dg::BIND_SHADER_RESOURCE,
            Dg::USAGE_DYNAMIC,
            Dg::CPU_ACCESS_WRITE,
            Dg::BUFFER_MODE_STRUCTURED,
            static_cast<uint32_t>(alignedEntityInstanceSize)
        };
        renderDevice->CreateBuffer(bufferDesc, nullptr, &entityInstanceBuffer);

        //

        Ptr<RendererSubmodule> rendererSubmodule;
        GetEngine().GetRegistry().GetModule(IID_GraphicsModule)->QueryInterface(IID_RendererSubmodule, rendererSubmodule.DblPtr<IObject>());

        rendererSubmodule->OnRenderUpdate(
            [mdl, deviceContext, technique, material, swapChain, entityInstanceBuffer, entityResourceBinding]
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

                auto bindAllInSrb = [](Dg::IShaderResourceBinding* srb, Dg::SHADER_TYPE type, const char* name,
                                       Dg::IDeviceObject* object, Dg::SET_SHADER_RESOURCE_FLAGS setFlags = Dg::SET_SHADER_RESOURCE_FLAG_NONE)
                {
                    uint32_t typeFlag = static_cast<uint32_t>(type);
                    uint32_t i        = 0;
                    while (typeFlag != 0)
                    {
                        if (typeFlag & 1)
                        {
                            auto var = srb->GetVariableByName(static_cast<Dg::SHADER_TYPE>(1 << i), name);
                            if (var)
                            {
                                var->Set(object, setFlags);
                            }
                        }
                        typeFlag >>= 1;
                        ++i;
                    }
                    srb->GetPipelineResourceSignature();
                };

                bindAllInSrb(entityResourceBinding, Dg::SHADER_TYPE_ALL_GRAPHICS, "g_Positions", mdl->GetPositionBuffer()->GetDefaultView(Dg::BUFFER_VIEW_SHADER_RESOURCE));
                bindAllInSrb(entityResourceBinding, Dg::SHADER_TYPE_ALL_GRAPHICS, "g_Normals", mdl->GetNormalBuffer()->GetDefaultView(Dg::BUFFER_VIEW_SHADER_RESOURCE));
                bindAllInSrb(entityResourceBinding, Dg::SHADER_TYPE_ALL_GRAPHICS, "g_TexCoords", mdl->GetTexCoordBuffer()->GetDefaultView(Dg::BUFFER_VIEW_SHADER_RESOURCE));
                bindAllInSrb(entityResourceBinding, Dg::SHADER_TYPE_ALL_GRAPHICS, "g_Tangents", mdl->GetTangentBuffer()->GetDefaultView(Dg::BUFFER_VIEW_SHADER_RESOURCE));

                {
                    Dg::MapHelper<EntityInstance> mappedData(deviceContext, entityInstanceBuffer, Dg::MAP_WRITE, Dg::MAP_FLAG_DISCARD);
                    uint32_t                      i = 0;
                    for (auto& submesh : mdl->GetSubMeshes())
                    {
                        EntityInstance instance{
                            submesh.PositionOffset,
                            submesh.NormalOffset,
                            submesh.TexCoordOffset,
                            submesh.TangentOffset
                        };
                        mappedData[i++] = instance;
                    }
                }
                bindAllInSrb(entityResourceBinding, Dg::SHADER_TYPE_ALL_GRAPHICS, "g_EntityInstances", entityInstanceBuffer->GetDefaultView(Dg::BUFFER_VIEW_SHADER_RESOURCE));

                deviceContext->CommitShaderResources(entityResourceBinding, Dg::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
                deviceContext->SetIndexBuffer(mdl->GetIndexBuffer(), 0, Dg::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);

                uint32_t i = 0;
                for (auto& submesh : mdl->GetSubMeshes())
                {
                    Dg::DrawIndexedAttribs drawAttribs(submesh.IndexCount, mdl->GetIndexType(), Dg::DRAW_FLAG_VERIFY_ALL, 1, submesh.IndexOffset, 0, i);
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
