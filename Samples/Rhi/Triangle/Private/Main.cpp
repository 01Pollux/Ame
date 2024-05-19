#include <ranges>

#include <Framework/EntryPoint.hpp>
#include <Framework/Window.hpp>

#include <Frame/Subsystem/Frame.hpp>
#include <Frame/Subsystem/Timer.hpp>

#include <Rhi/Resource/Shader.hpp>
#include <Rhi/CommandList/CommandList.hpp>
#include <Rhi/Resource/PipelineState.hpp>
#include <Rhi/Resource/Buffer.hpp>
#include <Rhi/Resource/VertexView.hpp>
#include <Rhi/Stream/Buffer.hpp>
#include <Math/Common.hpp>

#include <Log/Wrapper.hpp>

using namespace Ame;

class TriangleSampleEngine : public BaseEngine
{
    struct ViewportScissor
    {
        std::array<Rhi::Viewport, 1> Viewport;
        std::array<Rhi::Scissor, 1>  Scissor;
    };

protected:
    void Initialize() override
    {
        BaseEngine::Initialize();

        Log::Engine().Trace("Initializing Sample...");

        auto& RhiDevice = GetSubsystem<Rhi::DeviceSubsystem>();
        auto& Coroutine = *GetSubsystem<CoroutineSubsystem>();
        auto& Frame     = GetSubsystem<EngineFrameSubsystem>();

        m_PipelineStateTask = CreateBasicPipeline(
            Coroutine,
            RhiDevice);

        m_BufferTask = CreateBuffers(
            Coroutine,
            RhiDevice);

        m_OnUpdate = {
            Frame.OnRender()
                .ObjectSignal(),
            [this, &RhiDevice, &Timer = GetSubsystem<FrameTimerSubsystem>()]()
            {
                Render(Timer, RhiDevice);
            }
        };
    }

private:
    void Render(
        FrameTimer&  Timer,
        Rhi::Device& RhiDevice)
    {
        if (m_PipelineStateTask)
        {
            m_PipelineStateTask.wait();
        }

        Rhi::CommandList CommandList(RhiDevice);
        FinishUploadingResources(CommandList);

        CommandList.SetPipelineLayout(m_PipelineState->GetLayout());
        CommandList.SetPipelineState(m_PipelineState);

        Rhi::AttachmentsDesc     Attachments{};
        const Rhi::ResourceView* RenderTargets[]{
            &RhiDevice.GetBackbuffer().View
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
        Set.SetDynamicBuffer(RhiDevice, 0, m_DynamicBufferView.Unwrap());

        const nri::Descriptor* Descriptors[]{
            m_TextureView.Unwrap()
        };
        Set.SetRange(RhiDevice, 0, { .descriptors = Descriptors, .descriptorNum = Rhi::Count32(Descriptors) });

        Descriptors[0] = m_TextureSampler.Unwrap();
        Set.SetRange(RhiDevice, 1, { .descriptors = Descriptors, .descriptorNum = Rhi::Count32(Descriptors) });

        CommandList.SetDescriptorSet(0, Set, 0);

        auto [Viewports, Scissors] = GetViewportsAndScissors(RhiDevice);
        CommandList.SetViewports(Viewports);
        CommandList.SetScissorRects(Scissors);
        CommandList.SetVertexBuffer({ .Buffer = m_DrawBuffer, .Offset = VertexOffset });
        CommandList.SetIndexBuffer({ .Buffer = m_DrawBuffer, .Offset = IndexOffset, .Type = Rhi::IndexType::UINT16 });
        CommandList.Draw(Rhi::DrawIndexedDesc{ .indexNum = 3, .instanceNum = 1 });
        CommandList.EndRendering();
    }

private:
    [[nodiscard]] ViewportScissor GetViewportsAndScissors(
        Rhi::Device& RhiDevice)
    {
        auto& BackbufferDesc = RhiDevice.GetBackbuffer().Resource.GetDesc();

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

    [[nodiscard]] Co::result<std::vector<Rhi::ShaderBytecode>> LoadShaders(
        Co::executor_tag,
        Co::thread_pool_executor& Executor,
        Rhi::Device&              RhiDevice) const
    {
        std::vector<Rhi::ShaderBytecode> Shaders;
        Shaders.reserve(2);

        constexpr const char* SourceCode = R"(
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

        auto VertexShader = Rhi::ShaderBytecode::Compile({}, Executor, RhiDevice.GetGraphicsAPI(), SourceCode, { .Stage = Rhi::ShaderType::VERTEX_SHADER });
        auto PixelShader  = Rhi::ShaderBytecode::Compile({}, Executor, RhiDevice.GetGraphicsAPI(), SourceCode, { .Stage = Rhi::ShaderType::FRAGMENT_SHADER });

        Shaders.emplace_back(std::move(co_await VertexShader));
        Shaders.emplace_back(std::move(co_await PixelShader));

        co_return Shaders;
    }

    [[nodiscard]] Co::result<Ptr<Rhi::PipelineLayout>> LoadLayout(
        Co::executor_tag,
        Co::thread_pool_executor& Executor,
        Rhi::Device&              RhiDevice) const
    {
        Rhi::DynamicConstantBufferDesc Buffers[]{
            { .registerIndex = 0,
              .shaderStages  = Rhi::ShaderType::VERTEX_SHADER }
        };

        Rhi::DescriptorRangeDesc Ranges[]{
            { .descriptorNum  = 1,
              .descriptorType = Rhi::DescriptorType::TEXTURE,
              .shaderStages   = Rhi::ShaderType::FRAGMENT_SHADER },
            { .descriptorNum  = 1,
              .descriptorType = Rhi::DescriptorType::SAMPLER,
              .shaderStages   = Rhi::ShaderType::FRAGMENT_SHADER }
        };

        Rhi::DescriptorSetDesc DescriptorSets[]{
            { .registerSpace            = 1,
              .ranges                   = Ranges,
              .rangeNum                 = Rhi::Count32(Ranges),
              .dynamicConstantBuffers   = Buffers,
              .dynamicConstantBufferNum = Rhi::Count32(Buffers) }
        };

        Rhi::PipelineLayoutDesc Desc{
            .descriptorSets   = DescriptorSets,
            .descriptorSetNum = Rhi::Count32(DescriptorSets),
            .shaderStages     = Rhi::ShaderType::VERTEX_SHADER | Rhi::ShaderType::FRAGMENT_SHADER
        };
        co_return co_await RhiDevice.CreatePipelineLayout({}, Executor, Desc);
    }

    [[nodiscard]] Co::result<void> CreateBasicPipeline(
        Co::runtime& Coroutine,
        Rhi::Device& RhiDevice)
    {
        auto& Executor = *Coroutine.thread_pool_executor();

        auto ShaderTask = LoadShaders({}, Executor, RhiDevice);
        auto LayoutTask = LoadLayout({}, Executor, RhiDevice);

        Rhi::RenderTargetDesc RenderTargets[]{
            { RhiDevice.GetBackbuffer().Resource.GetDesc().format }
        };

        Rhi::VertexStreamDesc VertexStreams[]{
            { .stride      = sizeof(Vertices[0]),
              .bindingSlot = 0,
              .stepRate    = Rhi::VertexStreamStepRate::PER_VERTEX }
        };

        Rhi::VertexAttributeDesc VertexAttributes[]{
            { .d3d{ "POSITION", 0 },
              .vk{ 0 },
              .offset      = 0,
              .format      = Rhi::ResourceFormat::RG32_SFLOAT,
              .streamIndex = 0 }
        };

        Rhi::VertexInputDesc VertexInput{
            .attributes   = VertexAttributes,
            .streams      = VertexStreams,
            .attributeNum = Rhi::Count8(VertexAttributes),
            .streamNum    = Rhi::Count8(VertexStreams)
        };

        Rhi::GraphicsPipelineDesc Desc{
            .InputAssembly{ Rhi::TopologyType::TRIANGLE_LIST },
            .Rasterizer{ .Cull = Rhi::CullMode::NONE },
            .OutputMerger{ RenderTargets },
            .VertexInput = &VertexInput
        };

        Desc.Layout = co_await LayoutTask;

        auto Shaders     = co_await ShaderTask;
        auto ShaderDescs = Shaders |
                           std::views::transform([](const auto& Shader)
                                                 { return Shader.GetDesc(); }) |
                           std::ranges::to<std::vector>();

        Desc.Shaders = ShaderDescs;

        m_PipelineState = co_await RhiDevice.CreatePipelineState({}, Executor, Desc);
    }

private:
    static constexpr Math::Vector2 Vertices[]{
        { 0.0f, 0.5f },
        { 0.5f, -0.5f },
        { -0.5f, -0.5f }
    };

    static constexpr uint16_t Indices[]{
        0, 1, 2
    };
    std::array<uint8_t, 4> TextureData{ 255, 0, 0, 255 };

    static constexpr Rhi::BufferDesc BufferDesc{
        .size      = sizeof(Vertices) + sizeof(Indices),
        .usageMask = Rhi::BufferUsageBits::VERTEX_BUFFER | Rhi::BufferUsageBits::INDEX_BUFFER
    };

    static constexpr uint32_t IndexOffset  = sizeof(Vertices);
    static constexpr uint32_t VertexOffset = 0;

    static constexpr Rhi::TextureDesc TextureDesc = Rhi::Tex2D(
        Rhi::ResourceFormat::RGBA8_UNORM,
        1, 1, 1);

    static constexpr Rhi::BufferDesc DynamicDesc{
        .size      = 1024,
        .usageMask = Rhi::BufferUsageBits::CONSTANT_BUFFER
    };

    static constexpr Rhi::BufferDesc UploadDesc{
        .size = sizeof(TextureData) + BufferDesc.size
    };

    static constexpr Rhi::SamplerDesc SamplerDesc{
        .filters{ nri::Filter::LINEAR, nri::Filter::LINEAR, nri::Filter::LINEAR },
        .anisotropy = 8,
        .mipMax     = 16.0f,
        .addressModes{ nri::AddressMode::REPEAT, nri::AddressMode::REPEAT }
    };

    void AllocateResources(
        Rhi::Device& RhiDevice)
    {
        m_DrawBuffer    = Rhi::Buffer(RhiDevice, Rhi::MemoryLocation::DEVICE, BufferDesc);
        m_Texture       = Rhi::Texture(RhiDevice, Rhi::MemoryLocation::DEVICE, TextureDesc);
        m_DynamicBuffer = Rhi::Buffer(RhiDevice, Rhi::MemoryLocation::HOST_UPLOAD, DynamicDesc);
        m_TempBuffer    = Rhi::Buffer(RhiDevice, Rhi::MemoryLocation::HOST_UPLOAD, UploadDesc);

        m_DynamicBufferView = m_DynamicBuffer.CreateView({});
        m_TextureView       = m_Texture.CreateShaderView(Rhi::TextureViewDesc{ Rhi::TextureViewType::ShaderResource2D });
        m_TextureSampler    = Rhi::SamplerResourceView(RhiDevice, SamplerDesc);

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
        Co::thread_pool_executor& Executor,
        Rhi::Device&              RhiDevice)
    {
        AllocateResources(RhiDevice);
        UploadResources();
        co_return;
    }

    Co::result<void> CreateBuffers(
        Co::runtime& Coroutine,
        Rhi::Device& RhiDevice)
    {
        co_await CreateBuffers({}, *Coroutine.thread_pool_executor(), RhiDevice);
    }

private:
    Co::result<void> m_PipelineStateTask;
    Co::result<void> m_BufferTask;

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
