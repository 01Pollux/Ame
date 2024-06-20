#include <ranges>

#include <Framework/EntryPoint.hpp>
#include <Framework/Window.hpp>

#include <Rhi/Resource/Shader.hpp>
#include <Rhi/CommandList/CommandList.hpp>
#include <Rhi/Resource/PipelineState.hpp>
#include <Rhi/Resource/Buffer.hpp>
#include <Rhi/Resource/VertexView.hpp>
#include <Math/Common.hpp>

#include <Rhi/Device/CommandSubmitter.hpp>
#include <Rhi/Device/ResourceAllocator.hpp>
#include <Rhi/Device/DeviceWindowManager.hpp>

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

        co_return;
    }

protected:
    Co::null_result Initialize()
    {
        Log::Engine().Trace("Initializing Sample...");

        auto& rhiDevice   = GetSubsystem<Rhi::DeviceSubsystem>();
        auto& runtime     = GetSubsystem<CoroutineSubsystem>();
        auto& engineFrame = GetSubsystem<EngineFrameSubsystem>();

        m_PipelineState = co_await CreateBasicPipeline(
            runtime,
            rhiDevice);

        m_BufferTask = CreateBuffers(
            Coroutine,
            rhiDevice);

        m_OnUpdate = {
            Frame.OnRender()
                .ObjectSignal(),
            [this, &rhiDevice, &Timer = GetSubsystem<FrameTimerSubsystem>()]()
            {
                Render(Timer, rhiDevice);
            }
        };
    }

private:
    void Render(
        FrameTimer&  Timer,
        Rhi::Device& rhiDevice)
    {
        Rhi::CommandList CommandList(rhiDevice);
        FinishUploadingResources(CommandList);

        CommandList.SetPipelineLayout(m_PipelineState->GetLayout());
        CommandList.SetPipelineState(m_PipelineState);

        Rhi::AttachmentsDesc     Attachments{};
        const Rhi::ResourceView* RenderTargets[]{
            &rhiDevice.GetBackbuffer().View
        };
        CommandList.BeginRendering(RenderTargets);

        auto  BufferPtr = m_DynamicBuffer.GetPtr();
        float Data[]{
            static_cast<float>(Timer.GetEngineTime()), 0.5f,
            0.5f, -0.5f,
            -0.5f, -0.5f
        };
        std::memcpy(BufferPtr, Data, sizeof(Data));

        auto Set = CommandList.AllocateSets(0)[0];
        Set.SetDynamicBuffer(rhiDevice, 0, m_DynamicBufferView.Unwrap());

        const nri::Descriptor* Descriptors[]{
            m_TextureView.Unwrap()
        };
        Set.SetRange(rhiDevice, 0, { .descriptors = Descriptors, .descriptorNum = Rhi::Count32(Descriptors) });

        Descriptors[0] = m_TextureSampler.Unwrap();
        Set.SetRange(rhiDevice, 1, { .descriptors = Descriptors, .descriptorNum = Rhi::Count32(Descriptors) });

        CommandList.SetDescriptorSet(0, Set, 0);

        auto [Viewports, Scissors] = GetViewportsAndScissors(rhiDevice);
        CommandList.SetViewports(Viewports);
        CommandList.SetScissorRects(Scissors);
        CommandList.SetVertexBuffer({ .Buffer = m_DrawBuffer, .Offset = VertexOffset });
        CommandList.SetIndexBuffer({ .Buffer = m_DrawBuffer, .Offset = IndexOffset, .Type = Rhi::IndexType::UINT16 });
        CommandList.Draw(Rhi::DrawIndexedDesc{ .indexNum = 3, .instanceNum = 1 });
        CommandList.EndRendering();
    }

private:
    [[nodiscard]] ViewportScissor GetViewportsAndScissors(
        Rhi::Device& rhiDevice)
    {
        auto& BackbufferDesc = rhiDevice.GetBackbuffer().Resource.GetDesc();

        return ViewportScissor{
            .Viewport{
                Rhi::Viewport{
                    .width         = static_cast<float>(BackbufferDesc.width),
                    .height        = static_cast<float>(BackbufferDesc.height),
                    .depthRangeMax = 1.0f } },
            .Scissor{
                Rhi::Scissor{
                    .width  = BackbufferDesc.width,
                    .height = BackbufferDesc.height } }
        };
    }

private:
    [[nodiscard]] Co::result<std::vector<Rhi::ShaderBytecode>> LoadShaders(
        Co::executor&                 shaderExecutor,
        Rhi::DeviceResourceAllocator& resourceAllocator) const
    {
        std::vector<Rhi::ShaderBytecode> Shaders;
        Shaders.reserve(2);

        constexpr const char* sourceCode = R"(
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
            return ps.color * Texture.Sample(Sampler, float2(0.5, 0.5));
		}
		)";

        auto VertexShader = Rhi::ShaderBytecode::Compile({}, executor, rhiDevice.GetGraphicsAPI(), sourceCode, { .Stage = Rhi::ShaderType::VERTEX_SHADER });
        auto PixelShader  = Rhi::ShaderBytecode::Compile({}, executor, rhiDevice.GetGraphicsAPI(), sourceCode, { .Stage = Rhi::ShaderType::FRAGMENT_SHADER });

        Shaders.emplace_back(std::move(co_await VertexShader));
        Shaders.emplace_back(std::move(co_await PixelShader));

        co_return Shaders;
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

    [[nodiscard]] Co::result<Rhi::ScopedPipelineState> CreateBasicPipeline(
        Co::executor& shaderExecutor,
        Rhi::Device&  rhiDevice)
    {
        co_await Co::resume_on(rhiDevice.GetExecutor(Rhi::ExecutorType::Resources));

        auto& resourceAllocator = rhiDevice.GetResourceAllocator();

        auto shaderTask = LoadShaders(shaderExecutor, resourceAllocator);
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
            .InputAssembly{ Rhi::TopologyType::TRIANGLE_LIST },
            .Rasterizer{ .Cull = Rhi::CullMode::NONE },
            .OutputMerger{ renderTargets },
            .VertexInput = &vertexInput
        };

        m_PipelineLayout = co_await layoutTask;

        auto shaders      = co_await shaderTask;
        auto shadersDescs = shaders |
                            std::views::transform([](const auto& shader)
                                                  { return shader.GetDesc(); }) |
                            std::ranges::to<std::vector>();

        pipelineDesc.Shaders = shadersDescs;

        auto pipelineState = co_await resourceAllocator.CreatePipelineState(pipelineDesc);
        co_return Rhi::ScopedPipelineState(resourceAllocator, std::move(pipelineState));
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
    void AllocateResources(
        Rhi::Device& rhiDevice)
    {
        m_DrawBuffer    = Rhi::Buffer(rhiDevice, Rhi::MemoryLocation::DEVICE, c_BufferDesc);
        m_Texture       = Rhi::Texture(rhiDevice, Rhi::MemoryLocation::DEVICE, c_TextureDesc);
        m_DynamicBuffer = Rhi::Buffer(rhiDevice, Rhi::MemoryLocation::HOST_UPLOAD, c_DynamicDesc);
        m_TempBuffer    = Rhi::Buffer(rhiDevice, Rhi::MemoryLocation::HOST_UPLOAD, c_UploadDesc);

        m_DynamicBufferView = m_DynamicBuffer.CreateView({});
        m_TextureView       = m_Texture.CreateShaderView(Rhi::TextureViewDesc{ Rhi::TextureViewType::ShaderResource2D });
        m_TextureSampler    = Rhi::SamplerResourceView(rhiDevice, SamplerDesc);

        m_DrawBuffer.SetName("DrawBuffer");
        m_Texture.SetName("Texture");
        m_DynamicBuffer.SetName("DynamicBuffer");
        m_TempBuffer.SetName("TempBuffer");
    }

    void UploadResources()
    {
        namespace RS = Rhi::Streaming;
        RS::BufferOStream Stream(RS::BufferView(m_TempBuffer, Rhi::EntireBuffer));

        Stream.write(std::bit_cast<const char*>(&TextureData[0]), sizeof(TextureData));

        Stream.write(std::bit_cast<const char*>(&Vertices[0]), sizeof(Vertices));
        Stream.write(std::bit_cast<const char*>(&Indices[0]), sizeof(Indices));
    }

    void FinishUploadingResources(
        Rhi::CommandList& CommandList)
    {
        if (m_TempBuffer) [[unlikely]]
        {
            m_BufferTask.wait();

            CommandList.RequireState(m_Texture, { Rhi::AccessBits::COPY_DESTINATION, Rhi::LayoutType::COPY_DESTINATION });
            CommandList.CommitBarriers();

            CommandList.UploadTexture(
                { .RhiTexture = m_Texture,
                  .RhiBuffer  = m_TempBuffer,
                  .Layout{ .rowPitch = sizeof(TextureData) } });
            CommandList.CopyBuffer({ m_TempBuffer, Rhi::Size32(TextureData) }, { m_DrawBuffer });

            CommandList.RequireState(m_DrawBuffer, { Rhi::AccessBits::VERTEX_BUFFER | Rhi::AccessBits::INDEX_BUFFER });
            CommandList.RequireState(m_Texture, { Rhi::AccessBits::SHADER_RESOURCE, Rhi::LayoutType::SHADER_RESOURCE });
            CommandList.RequireState(m_DynamicBuffer, { Rhi::AccessBits::CONSTANT_BUFFER });
            CommandList.CommitBarriers();

            m_TempBuffer = nullptr;
        }
    }

    Co::result<void> CreateBuffers(
        Co::executor_tag,
        Co::thread_pool_executor& executor,
        Rhi::Device&              rhiDevice)
    {
        AllocateResources(rhiDevice);
        UploadResources();
        co_return;
    }

    Co::result<void> CreateBuffers(
        Co::runtime& Coroutine,
        Rhi::Device& rhiDevice)
    {
        co_await CreateBuffers({}, *Coroutine.thread_pool_executor(), rhiDevice);
    }

private:
    Rhi::ScopedPipelineLayout m_PipelineLayout;
    Rhi::ScopedPipelineState  m_PipelineState;

    Ptr<Rhi::PipelineState> m_PipelineState;

    Rhi::Buffer m_DrawBuffer;

    Rhi::Buffer             m_DynamicBuffer;
    Rhi::BufferResourceView m_DynamicBufferView;

    Rhi::Texture             m_Texture;
    Rhi::ShaderResourceView  m_TextureView;
    Rhi::SamplerResourceView m_TextureSampler;

    Rhi::Buffer m_TempBuffer;

    Signals::OnUpdate::Handle m_OnUpdate;
};

AME_MAIN(Argc, Argv)
{
    Log::Logger::Register(Log::Names::Engine, "Engine.log");
    Log::Logger::Register(Log::Names::Rhi, "Engine.log");

    WindowApplication<TriangleSampleEngine>::Builder()
        .Title("Triangle")
        .Build()
        .Run();
}
