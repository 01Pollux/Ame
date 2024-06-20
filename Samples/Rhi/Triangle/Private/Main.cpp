#include <ranges>

#include <Framework/EntryPoint.hpp>
#include <Framework/Window.hpp>

#include <Math/Common.hpp>

#include <Rhi/Device/CommandSubmitter.hpp>
#include <Rhi/Device/ResourceAllocator.hpp>
#include <Rhi/Device/WindowManager.hpp>

#include <Rhi/Shader/Shader.Compiler.hpp>

#include <Rhi/Resource/PipelineState.hpp>
#include <Rhi/Resource/Buffer.hpp>
#include <Rhi/Resource/Texture.hpp>
#include <Rhi/Resource/View.hpp>
#include <Rhi/Resource/DescriptorTable.hpp>
#include <Rhi/Resource/VertexView.hpp>
#include <Rhi/Descs/View.hpp>

#include <Log/Wrapper.hpp>

using namespace Ame;

class TriangleSampleEngine : public IoCContainer
{
    struct ViewportScissor
    {
        std::array<Rhi::Viewport, 1> Viewport;
        std::array<Rhi::Scissor, 1>  Scissor;
    };

public:
    Co::result<void> Run()
    {
        UpdateOnFrames();
        Initialize();

        auto& engineFrame  = GetSubsystem<EngineFrameSubsystem>();
        auto& rhiDevice    = GetSubsystem<Rhi::DeviceSubsystem>();
        auto  thisExecutor = GetSubsystem<CoroutineSubsystem>().inline_executor();

        while (engineFrame.IsRunning())
        {
            auto frameTick   = thisExecutor->submit(std::bind(&EngineFrame::Tick, &engineFrame));
            auto frameRender = thisExecutor->submit(std::bind(&Rhi::Device::Tick, &rhiDevice));

            Co::when_all(thisExecutor, std::move(frameTick), std::move(frameRender)).run().wait();
        }

        m_DeviceResource = {};
        co_return;
    }

protected:
    Co::null_result Initialize()
    {
        Log::Engine().Trace("Initializing Sample...");

        auto& rhiDevice = GetSubsystem<Rhi::DeviceSubsystem>();
        auto& runtime   = GetSubsystem<CoroutineSubsystem>();

        auto pipelineStateTask = CreateBasicPipeline(
            runtime.background_executor(),
            rhiDevice);

        auto bufferTask = CreateBuffers(rhiDevice);
        co_await Co::when_all(runtime.background_executor(), std::move(pipelineStateTask), std::move(bufferTask)).run();

        m_ReadyToRender = true;
    }

private:
    Co::null_result UpdateOnFrames()
    {
        auto& engineFrame = GetSubsystem<EngineFrameSubsystem>();
        auto& rhiDevice   = GetSubsystem<Rhi::DeviceSubsystem>();
        auto& frameTimer  = GetSubsystem<FrameTimerSubsystem>();

        Log::Client().Info("Waiting for engine frame...");

        auto& commandSubmitter = rhiDevice.GetCommandSubmitter();
        auto& windowManager    = rhiDevice.GetWindowManager();

        while (engineFrame.IsRunning())
        {
            co_await Co::resume_on(engineFrame.GetStartFrameExecutor());
            co_await Co::resume_on(rhiDevice.GetExecutor());

            auto fence = co_await ClearFrame(rhiDevice);
            fence      = co_await Render(frameTimer, rhiDevice, fence);
            fence      = co_await RenderFrame(rhiDevice, fence);
        }
    }

    [[nodiscard]] Co::result<Rhi::Fence> ClearFrame(
        Rhi::Device& rhiDevice)
    {
        auto& commandSubmitter = rhiDevice.GetCommandSubmitter();
        auto& windowManager    = rhiDevice.GetWindowManager();

        auto  submission = co_await commandSubmitter.BeginCommandList(Rhi::CommandQueueType::GRAPHICS);
        auto& backbuffer = (co_await windowManager.GetBackbuffer()).get();

        Rhi::TextureBarrierDesc backbufferTextureBarrier{
            .texture = backbuffer.Resource.Unwrap(),
            .before{ .access = Rhi::AccessBits::UNKNOWN, .layout = Rhi::LayoutType::UNKNOWN, .stages = Rhi::ShaderType::ALL },
            .after{ .access = Rhi::AccessBits::COLOR_ATTACHMENT, .layout = Rhi::LayoutType::COLOR_ATTACHMENT, .stages = Rhi::ShaderType::ALL }
        };
        Rhi::BarrierGroupDesc backbufferBarrier{
            .textures   = &backbufferTextureBarrier,
            .textureNum = 1
        };

        submission->ResourceBarrier(backbufferBarrier);

        {
            std::array renderTargets{ &backbuffer.View };
            std::array clearColors{ Rhi::ClearDesc::RenderTarget(Colors::c_GreenYellow) };

            Rhi::RenderingCommand rendering(submission.CommandListRef, renderTargets);
            submission->ClearAttachments(clearColors);
        }

        commandSubmitter.SubmitCommandList(submission);

        co_return submission.FenceRef;
    }

    [[nodiscard]] Co::result<Rhi::Fence> RenderFrame(
        Rhi::Device&      rhiDevice,
        const Rhi::Fence& fence)
    {
        auto& commandSubmitter = rhiDevice.GetCommandSubmitter();
        auto& windowManager    = rhiDevice.GetWindowManager();

        auto  submission = co_await commandSubmitter.BeginCommandList(Rhi::CommandQueueType::GRAPHICS);
        auto& backbuffer = (co_await windowManager.GetBackbuffer()).get();

        Rhi::TextureBarrierDesc backbufferTextureBarrier{
            .texture = backbuffer.Resource.Unwrap(),
            .before{ .access = Rhi::AccessBits::COLOR_ATTACHMENT, .layout = Rhi::LayoutType::COLOR_ATTACHMENT, .stages = Rhi::ShaderType::ALL },
            .after{ .access = Rhi::AccessBits::UNKNOWN, .layout = Rhi::LayoutType::PRESENT, .stages = Rhi::ShaderType::ALL }
        };
        Rhi::BarrierGroupDesc backbufferBarrier{
            .textures   = &backbufferTextureBarrier,
            .textureNum = 1
        };

        submission->ResourceBarrier(backbufferBarrier);

        commandSubmitter.SubmitCommandList(submission, { fence });

        co_return submission.FenceRef;
    }

private:
    [[nodiscard]] Co::result<Rhi::Fence> Render(
        FrameTimer&       frameTimer,
        Rhi::Device&      rhiDevice,
        const Rhi::Fence& fence)
    {
        if (!m_ReadyToRender)
        {
            co_return fence;
        }

        auto& commandSubmitter = rhiDevice.GetCommandSubmitter();
        auto& windowManager    = rhiDevice.GetWindowManager();
        auto& backbuffer       = (co_await windowManager.GetBackbuffer()).get();

        auto submission = co_await commandSubmitter.BeginCommandList(Rhi::CommandQueueType::GRAPHICS);
        {
            std::array renderTargets{ &backbuffer.View };

            Rhi::RenderingCommand rendering(submission.CommandListRef, renderTargets);

            submission->SetDescriptorTable(*m_DeviceResource.DescriptorTable.Unwrap());
            submission->SetPipelineLayout(*m_DeviceResource.PipelineLayout.Unwrap());
            submission->SetPipelineState(*m_DeviceResource.PipelineState.Unwrap());

            auto  bufferPtr = m_DeviceResource.DynamicBuffer.GetPtr();
            float rata[]{
                static_cast<float>(frameTimer.GetEngineTime()), 0.5f,
                0.5f, -0.5f,
                -0.5f, -0.5f
            };
            std::memcpy(bufferPtr, rata, sizeof(rata));

            uint32_t constantBufferOffsets = 0;
            submission->SetDescriptorSet(0, *m_DeviceResource.DescriptorSet.Unwrap(), &constantBufferOffsets);

            auto [viewports, scissors] = GetViewportsAndScissors(backbuffer);
            submission->SetViewports(viewports);
            submission->SetScissorRects(scissors);
            submission->SetVertexBuffer({ .NriBuffer = m_DeviceResource.DrawBuffer.Unwrap(), .Offset = c_VertexOffset });
            submission->SetIndexBuffer({ .NriBuffer = m_DeviceResource.DrawBuffer.Unwrap(), .Offset = c_IndexOffset, .Type = Rhi::IndexType::UINT16 });
            submission->Draw(Rhi::DrawIndexedDesc{ .indexNum = 3, .instanceNum = 1 });
        }
        commandSubmitter.SubmitCommandList(submission, { fence });

        co_return submission.FenceRef;
    }

private:
    [[nodiscard]] ViewportScissor GetViewportsAndScissors(
        const Rhi::Backbuffer& backbuffer)
    {
        auto& backbufferDesc = backbuffer.Resource.GetDesc();
        return ViewportScissor{
            .Viewport{
                Rhi::Viewport{
                    .width         = static_cast<float>(backbufferDesc.width),
                    .height        = static_cast<float>(backbufferDesc.height),
                    .depthRangeMax = 1.0f } },
            .Scissor{
                Rhi::Scissor{
                    .width  = backbufferDesc.width,
                    .height = backbufferDesc.height } }
        };
    }

private:
    [[nodiscard]] Co::result<std::vector<Rhi::ShaderBytecode>> LoadShaders(
        const Ptr<Co::executor>& shaderExecutor,
        const Rhi::DeviceDesc&   rhiDeviceDesc) const
    {
        std::vector<Rhi::ShaderBytecode> shaders;
        shaders.reserve(2);

        const StringView sourceCode = R"(
        struct VSInput
        {
        	float2 pos : POSITION;  
        };
        struct VSOutput
        {
            float4 pos : SV_POSITION;
            float4 color : COLOR;
        };
        struct ConstantData
        {
            float time;
        };

		ConstantBuffer<ConstantData> Data : register(b0, space1);
        Texture2D<float4> Texture : register(t0, space1);
        SamplerState Sampler : register(s0, space1);

        VSOutput VS_Main(VSInput vs) 
        {
            VSOutput ps;
			ps.pos = float4(vs.pos, 0.0, 1.0);
			ps.color = float4(sin(Data.time), sin(Data.time + 2.0f), sin(Data.time + 4.0f), 1.0f);
			return ps;
        }
        float4 PS_Main(VSOutput ps) : SV_TARGET
        {
            return float4(1.0, 0.0, 0.0, 1.0);
            //return ps.color * Texture.Sample(Sampler, float2(0.5, 0.5));
		}
		)";

        auto compileShader = [&](Rhi::ShaderCompileStage stage)
        {
            Rhi::ShaderCompilerDesc compilerDesc{ .DeviceDesc = rhiDeviceDesc };
            compilerDesc.SetAsShader(stage);

            Rhi::ShaderCompileDesc compileDesc{
                .CompilerDesc = compilerDesc,
                .SourceCode   = sourceCode
            };

            return Rhi::ShaderCompiler::Compile(compileDesc);
        };

        auto [vertexShader, pixelShader] = co_await Co::when_all(
            shaderExecutor,
            co_await shaderExecutor->submit(compileShader, Rhi::ShaderCompileStage::Vertex).resolve(),
            co_await shaderExecutor->submit(compileShader, Rhi::ShaderCompileStage::Pixel).resolve());

        shaders.emplace_back(std::move(co_await vertexShader));
        shaders.emplace_back(std::move(co_await pixelShader));

        co_return shaders;
    }

    [[nodiscard]] Co::result<Rhi::ScopedPipelineLayout> LoadLayout(
        Rhi::DeviceResourceAllocator& resourceAllocator) const
    {
        Rhi::DynamicConstantBufferDesc buffers[]{
            { .registerIndex = 0,
              .shaderStages  = Rhi::ShaderType::VERTEX_SHADER }
        };

        Rhi::DescriptorRangeDesc ranges[]{
            { .descriptorNum  = 1,
              .descriptorType = Rhi::DescriptorType::TEXTURE,
              .shaderStages   = Rhi::ShaderType::FRAGMENT_SHADER },
            { .descriptorNum  = 1,
              .descriptorType = Rhi::DescriptorType::SAMPLER,
              .shaderStages   = Rhi::ShaderType::FRAGMENT_SHADER }
        };

        Rhi::DescriptorSetDesc descriptorSets[]{
            { .registerSpace            = 1,
              .ranges                   = ranges,
              .rangeNum                 = Rhi::Count32(ranges),
              .dynamicConstantBuffers   = buffers,
              .dynamicConstantBufferNum = Rhi::Count32(buffers) }
        };

        Rhi::PipelineLayoutDesc desc{
            .descriptorSets   = descriptorSets,
            .descriptorSetNum = Rhi::Count32(descriptorSets),
            .shaderStages     = Rhi::ShaderType::VERTEX_SHADER | Rhi::ShaderType::FRAGMENT_SHADER
        };

        auto pipelineLayout = co_await resourceAllocator.CreatePipelineLayout(desc);
        co_return Rhi::ScopedPipelineLayout(resourceAllocator, std::move(pipelineLayout));
    }

    [[nodiscard]] Co::result<void> CreateBasicPipeline(
        const Ptr<Co::executor>& shaderExecutor,
        Rhi::Device&             rhiDevice)
    {
        co_await Co::resume_on(rhiDevice.GetExecutor(Rhi::ExecutorType::Resources));

        auto& resourceAllocator = rhiDevice.GetResourceAllocator();

        auto shaderTask = LoadShaders(shaderExecutor, rhiDevice.GetDesc());
        auto layoutTask = LoadLayout(resourceAllocator);

        auto backbufferFormat = rhiDevice.GetWindowManager().GetBackbufferFormat();

        Rhi::RenderTargetDesc renderTargets[]{
            { backbufferFormat }
        };

        Rhi::VertexStreamDesc vertexStreams[]{
            { .stride      = sizeof(c_Vertices[0]),
              .bindingSlot = 0,
              .stepRate    = Rhi::VertexStreamStepRate::PER_VERTEX }
        };

        Rhi::VertexAttributeDesc vertexAttributes[]{
            { .d3d{ "POSITION", 0 },
              .vk{ 0 },
              .offset      = 0,
              .format      = Rhi::ResourceFormat::RG32_SFLOAT,
              .streamIndex = 0 }
        };

        Rhi::VertexInputDesc vertexInput{
            .attributes   = vertexAttributes,
            .streams      = vertexStreams,
            .attributeNum = Rhi::Count8(vertexAttributes),
            .streamNum    = Rhi::Count8(vertexStreams)
        };

        Rhi::GraphicsPipelineDesc pipelineDesc{
            .Layout = &m_DeviceResource.PipelineLayout,
            .InputAssembly{ Rhi::TopologyType::TRIANGLE_LIST },
            .Rasterizer{ .Cull = Rhi::CullMode::NONE },
            .OutputMerger{ renderTargets },
            .VertexInput = &vertexInput
        };

        m_DeviceResource.PipelineLayout = co_await layoutTask;

        auto shaders      = co_await shaderTask;
        auto shadersDescs = shaders |
                            std::views::transform([](const auto& shader)
                                                  { return shader.GetDesc(); }) |
                            std::ranges::to<std::vector>();

        pipelineDesc.Shaders = shadersDescs;

        auto pipelineState             = co_await resourceAllocator.CreatePipelineState(pipelineDesc);
        m_DeviceResource.PipelineState = Rhi::ScopedPipelineState(resourceAllocator, std::move(pipelineState));
    }

private:
    static constexpr Math::Vector2 c_Vertices[]{
        { 0.0f, 0.5f },
        { 0.5f, -0.5f },
        { -0.5f, -0.5f }
    };

    static constexpr uint16_t c_Indices[]{
        0, 1, 2
    };
    std::array<uint8_t, 4> c_TextureData{ 255, 0, 0, 255 };

    static constexpr Rhi::BufferDesc c_BufferDesc{
        .size      = sizeof(c_Vertices) + sizeof(c_Indices),
        .usageMask = Rhi::BufferUsageBits::VERTEX_BUFFER | Rhi::BufferUsageBits::INDEX_BUFFER
    };

    static constexpr uint32_t c_IndexOffset  = sizeof(c_Vertices);
    static constexpr uint32_t c_VertexOffset = 0;

    static constexpr Rhi::TextureDesc c_TextureDesc = Rhi::Tex2D(
        Rhi::ResourceFormat::RGBA8_UNORM,
        1, 1, 1);

    static constexpr Rhi::BufferDesc c_DynamicDesc{
        .size      = 1024,
        .usageMask = Rhi::BufferUsageBits::CONSTANT_BUFFER
    };

    static constexpr Rhi::BufferDesc c_UploadDesc{
        .size = sizeof(c_TextureData) + c_BufferDesc.size
    };

    static constexpr Rhi::SamplerDesc c_SamplerDesc{
        .filters{ nri::Filter::LINEAR, nri::Filter::LINEAR, nri::Filter::LINEAR },
        .anisotropy = 8,
        .mipMax     = 16.0f,
        .addressModes{ nri::AddressMode::REPEAT, nri::AddressMode::REPEAT }
    };

private:
    [[nodiscard]] Co::result<void> AllocateResources(
        Rhi::DeviceResourceAllocator& resourceAllocator)
    {
        m_DeviceResource.DrawBuffer    = { resourceAllocator, co_await resourceAllocator.CreateBuffer(c_BufferDesc, Rhi::MemoryLocation::DEVICE) };
        m_DeviceResource.Texture       = { resourceAllocator, co_await resourceAllocator.CreateTexture(c_TextureDesc) };
        m_DeviceResource.DynamicBuffer = { resourceAllocator, co_await resourceAllocator.CreateBuffer(c_DynamicDesc, Rhi::MemoryLocation::HOST_UPLOAD) };

        m_DeviceResource.DynamicBufferView = { resourceAllocator, co_await resourceAllocator.CreateView(m_DeviceResource.DynamicBuffer, {}) };
        m_DeviceResource.TextureView       = { resourceAllocator, co_await resourceAllocator.CreateView(m_DeviceResource.Texture, Rhi::TextureViewDesc{
                                                                                                                                      .Type   = Rhi::TextureViewType::ShaderResource2D,
                                                                                                                                      .Format = c_TextureDesc.format }) };
        m_DeviceResource.TextureSampler    = { resourceAllocator, co_await resourceAllocator.CreateSampler(c_SamplerDesc) };

        m_DeviceResource.DescriptorTable = {
            resourceAllocator,
            co_await resourceAllocator.CreateDescriptorTable(
                { .descriptorSetMaxNum         = 1, // one descriptor set, containting 1 sampler, 2 constant buffer, and one texture
                  .samplerMaxNum               = 1,
                  .dynamicConstantBufferMaxNum = 2,
                  .textureMaxNum               = 1 })
        };

        m_DeviceResource.DescriptorSet = m_DeviceResource.DescriptorTable.AllocateSet(*m_DeviceResource.PipelineLayout.Unwrap(), 0);
        {
            m_DeviceResource.DescriptorSet.SetDynamicBuffer(0, m_DeviceResource.DynamicBufferView.Unwrap());

            const nri::Descriptor* descriptors[]{ m_DeviceResource.TextureView.Unwrap() };
            m_DeviceResource.DescriptorSet.SetRange(0, { .descriptors = descriptors, .descriptorNum = Rhi::Count32(descriptors) });

            descriptors[0] = m_DeviceResource.TextureSampler.Unwrap();
            m_DeviceResource.DescriptorSet.SetRange(1, { .descriptors = descriptors, .descriptorNum = Rhi::Count32(descriptors) });
        }

        m_DeviceResource.DrawBuffer.SetName("DrawBuffer");
        m_DeviceResource.Texture.SetName("Texture");
        m_DeviceResource.DynamicBuffer.SetName("DynamicBuffer");
    }

    [[nodiscard]] Co::result<void> UploadResources(
        Rhi::Buffer& buffer)
    {
        auto   ptr    = buffer.GetPtr();
        size_t offset = 0;

        std::memcpy(ptr, c_TextureData.data(), sizeof(c_TextureData));
        ptr += sizeof(c_TextureData);

        std::memcpy(ptr, c_Vertices, sizeof(c_Vertices));
        ptr += sizeof(c_Vertices);

        std::memcpy(ptr, c_Indices, sizeof(c_Indices));

        co_return;
    }

    [[nodiscard]] Co::result<void> FinishUploadingResources(
        const Rhi::DeviceDesc&        deviceDesc,
        Rhi::SubmissionContext&       submissionContext,
        Rhi::DeviceResourceAllocator& resourceAllocator)
    {
        Rhi::ScopedBuffer tempBuffer{ resourceAllocator, co_await resourceAllocator.CreateBuffer(c_UploadDesc, Rhi::MemoryLocation::HOST_UPLOAD) };
        tempBuffer.SetName("TempBuffer");

        co_await UploadResources(tempBuffer);

        Rhi::TextureBarrierDesc textureBarrier{
            .texture = m_DeviceResource.Texture.Unwrap(),
            .before{ Rhi::AccessBits::UNKNOWN, Rhi::LayoutType::UNKNOWN, Rhi::StageBits::NONE },
            .after{ Rhi::AccessBits::COPY_DESTINATION, Rhi::LayoutType::COPY_DESTINATION, Rhi::StageBits::ALL }
        };

        Rhi::BufferBarrierDesc bufferBarriers[]{
            {
                .buffer = m_DeviceResource.DrawBuffer.Unwrap(),
                .before{ Rhi::AccessBits::UNKNOWN, Rhi::StageBits::NONE },
                .after{ Rhi::AccessBits::COPY_DESTINATION, Rhi::StageBits::COPY },
            },
            {
                .buffer = tempBuffer.Unwrap(),
                .before{ Rhi::AccessBits::UNKNOWN, Rhi::StageBits::NONE },
                .after{ Rhi::AccessBits::COPY_SOURCE, Rhi::StageBits::COPY },
            }
        };

        Rhi::BarrierGroupDesc barrierDesc{
            .buffers    = bufferBarriers,
            .textures   = &textureBarrier,
            .bufferNum  = Rhi::Count32(bufferBarriers),
            .textureNum = 1
        };

        submissionContext->ResourceBarrier(barrierDesc);

        submissionContext->UploadTexture(
            deviceDesc,
            {
                .NriTexture = m_DeviceResource.Texture.Unwrap(),
                .NriBuffer  = tempBuffer.Unwrap(),
                .TextureRegion{
                    .width  = c_TextureDesc.width,
                    .height = c_TextureDesc.height,
                    .depth  = c_TextureDesc.depth },
            });
        // std::memcpy(m_DeviceResource.DrawBuffer.GetPtr(), tempBuffer.GetPtr(sizeof(c_TextureData)), 30);
        submissionContext->CopyBuffer({ tempBuffer.Unwrap(), sizeof(c_TextureData) }, { m_DeviceResource.DrawBuffer.Unwrap(), 0 }, 30);

        textureBarrier.before    = std::exchange(textureBarrier.after, { Rhi::AccessBits::SHADER_RESOURCE, Rhi::LayoutType::SHADER_RESOURCE, Rhi::StageBits::FRAGMENT_SHADER });
        bufferBarriers[0].before = std::exchange(bufferBarriers[0].after, { Rhi::AccessBits::VERTEX_BUFFER | Rhi::AccessBits::INDEX_BUFFER, Rhi::StageBits::ALL });
        bufferBarriers[1]        = {
                   .buffer = m_DeviceResource.DynamicBuffer.Unwrap(),
                   .before{ Rhi::AccessBits::UNKNOWN, Rhi::StageBits::NONE },
                   .after{ Rhi::AccessBits::CONSTANT_BUFFER, Rhi::StageBits::ALL }
        };

        submissionContext->ResourceBarrier(barrierDesc);
    }

    [[nodiscard]] Co::result<void> FinishUploadingResources(
        Rhi::Device& rhiDevice)
    {
        auto& commandSubmitter  = rhiDevice.GetCommandSubmitter();
        auto& resourceAllocator = rhiDevice.GetResourceAllocator();

        co_await Co::resume_on(rhiDevice.GetExecutor(Rhi::ExecutorType::Graphics));

        auto submissionContext = co_await commandSubmitter.BeginCommandList(Rhi::CommandQueueType::GRAPHICS);

        co_await FinishUploadingResources(rhiDevice.GetDesc(), submissionContext, resourceAllocator);

        co_await commandSubmitter.SubmitCommandList(std::move(submissionContext));
    }

private:
    [[nodiscard]] Co::result<void> CreateBuffers(
        Rhi::Device& rhiDevice)
    {
        co_await Co::resume_on(rhiDevice.GetExecutor(Rhi::ExecutorType::Resources));
        co_await AllocateResources(rhiDevice.GetResourceAllocator());
        co_await FinishUploadingResources(rhiDevice);
    }

private:
    struct DeviceResource
    {
        Rhi::ScopedPipelineLayout PipelineLayout;
        Rhi::ScopedPipelineState  PipelineState;

        Rhi::ScopedDescriptorTable DescriptorTable;
        Rhi::DescriptorSet         DescriptorSet;

        Rhi::ScopedBuffer DrawBuffer;

        Rhi::ScopedBuffer       DynamicBuffer;
        Rhi::ScopedResourceView DynamicBufferView;

        Rhi::ScopedTexture      Texture;
        Rhi::ScopedResourceView TextureView;
        Rhi::ScopedResourceView TextureSampler;
    };

    DeviceResource m_DeviceResource;
    bool           m_ReadyToRender = false;
};

AME_MAIN(Argc, Argv)
{
    Log::Logger::Register(Log::Names::c_Engine, "Engine.log");
    Log::Logger::Register(Log::Names::c_Rhi, "Engine.log");

    WindowApplication<TriangleSampleEngine>::Builder()
        //.RendererBackend(Rhi::DeviceType::DirectX12)
        .Title("Triangle")
        .Build()
        .Run();
}
