#include <ranges>

#include <Rhi/Resource/VertexView.hpp>
#include <Gfx/RenderGraph/Passes/ForwardOpaquePass.hpp>

#include <Gfx/Constants.hpp>
#include <Gfx/Shading/Material.hpp>

namespace Ame::Gfx
{
    using namespace EnumBitOperators;
    namespace CD = Constants::DescriptorRanges;

    //

    ForwardOpaquePass::ForwardOpaquePass(
        Cache::CommonShader&         commonShaders,
        Cache::MaterialBindingCache& materialCache) :
        m_CommonShaders(commonShaders),
        m_MaterialCache(materialCache)
    {
        m_CommonShaders.get().Load(Cache::CommonShader::Type::GBufferPass_PS);

        Name("ForwardOpaquePass")
            .SetFlags(RG::PassFlags::Graphics)
            .Build(
                [this](RG::Resolver& resolver)
                {
                    auto& frameData       = resolver.GetFrameResourceData();
                    auto  textureDesc     = resolver.GetBackbufferDesc();
                    textureDesc.usageMask = {};

                    resolver.CreateTexture(Output::c_OutputImage, textureDesc);
                    resolver.WriteRenderTarget(Output::c_OutputImage("Main"), Rhi::StageBits::DRAW, textureDesc.format);

                    //

                    resolver.ReadBuffer(
                        RG::Names::c_TransformsTable("GBufferPass"),
                        Rhi::StageBits::GRAPHICS_SHADERS);
                    resolver.ReadBuffer(
                        RG::Names::c_AABBTable("GBufferPass"),
                        Rhi::StageBits::GRAPHICS_SHADERS);
                })
            .Execute(
                [this](const RG::ResourceStorage& storage, Rhi::CommandList* commandList)
                {
                    auto& frameData      = storage.GetFrameResourceData();
                    auto& backbufferDesc = storage.GetBackbufferDesc();

                    auto& transformsTable = storage.GetResourceViewHandle(RG::Names::c_TransformsTable("GBufferPass"));
                    auto& aabbTable       = storage.GetResourceViewHandle(RG::Names::c_AABBTable("GBufferPass"));

                    //

                    nri::Descriptor* frameDescriptors[]{
                        storage.GetFrameResourceHandle().Unwrap()
                    };

                    nri::Descriptor* entityDescriptors[]{
                        transformsTable.Unwrap(),
                        aabbTable.Unwrap()
                    };

                    //

                    Shading::MaterialShaderLink gBufferShaders;

                    gBufferShaders.Shaders.emplace_back(m_CommonShaders.get().Load(Cache::CommonShader::Type::GBufferPass_PS).get().Borrow());
                    gBufferShaders.CompileDesc.SetStage(Rhi::ShaderType::FRAGMENT_SHADER);

                    //

                    std::array renderTargets{
                        backbufferDesc.format
                    };

                    Shading::MaterialRenderState RenderState{
                        renderTargets,
                        Rhi::ResourceFormat::UNKNOWN,
                        std::move(gBufferShaders)
                    };

                    // auto entityStore = storage.GetEntityStore();

                    Rhi::DescriptorSet frameDataSet;
                    Rhi::DescriptorSet entityDataSet;

                    Rhi::Viewport viewports[3]{
                        { .width         = frameData.Viewport.x(),
                          .height        = frameData.Viewport.y(),
                          .depthRangeMax = 1.f }
                    };
                    Rhi::ScissorRect scissors[3]{
                        { .width  = static_cast<Rhi::Dim_t>(frameData.Viewport.x()),
                          .height = static_cast<Rhi::Dim_t>(frameData.Viewport.y()) }
                    };

                    for (uint32_t i = 1; i < std::size(viewports); i++)
                    {
                        viewports[i] = viewports[0];
                        scissors[i]  = scissors[0];
                    }

                    commandList->SetViewports(viewports);
                    commandList->SetScissorRects(scissors);

                    /* for (auto& row : entityStore.GetCountedRows())
                     {
                         m_MaterialCache.get().Bind(*commandList, *row->Material);

                         if (!frameDataSet)
                         {
                             frameDataSet  = commandList->AllocateSet(CD::c_FrameData_SetIndex);
                             entityDataSet = commandList->AllocateSet(CD::c_EntityData_SetIndex);

                             frameDataSet.SetRange(0, { frameDescriptors, Rhi::Count32(frameDescriptors) });
                             entityDataSet.SetRange(0, { entityDescriptors, Rhi::Count32(entityDescriptors) });

                             commandList->SetDescriptorSet(CD::c_FrameData_SetIndex, frameDataSet);
                             commandList->SetDescriptorSet(CD::c_EntityData_SetIndex, entityDataSet);
                         }

                         auto pipelineState = row->Material->GetPipelineState(RenderState).get();
                         commandList->SetPipelineState(pipelineState);

                         commandList->SetVertexBuffer({ .Buffer = row->VtxBuffer });
                         commandList->SetIndexBuffer({ .Buffer = row->IdxBuffer, .Type = row->IndexType });

                         commandList->DrawIndirectIndexed(
                             { .DrawBuffer    = commandsBuffer.AsBuffer()->Unwrap(),
                               .DrawOffset    = row.DrawOffset,
                               .MaxDrawCount  = row->Count,
                               .CounterBuffer = counterBuffer.AsBuffer()->Unwrap(),
                               .CounterOffset = row.CounterOffset });
                     }*/
                });
    }
} // namespace Ame::Gfx