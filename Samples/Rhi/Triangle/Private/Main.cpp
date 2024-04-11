#include <ranges>

#include <Framework/EntryPoint.hpp>
#include <Framework/Window.hpp>

#include <Rhi/Resource/Shader.hpp>
#include <Rhi/Resource/CommandList.hpp>
#include <Rhi/Resource/PipelineState.hpp>

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

        m_PipelineStateTask = CreateBasicPipeline(
            *GetSubsystem<CoroutineSubsystem>(),
            GetSubsystem<Rhi::DeviceSubsystem>());

        OnRender().ObjectSignal().Listen(
            [this](BaseEngine& Engine)
            {
                Render(
                    GetSubsystem<Rhi::DeviceSubsystem>());
            });
    }

    void Shutdown() override
    {
        m_PipelineState = nullptr;
    }

private:
    void Render(
        Rhi::Device& RhiDevice)
    {
        if (m_PipelineStateTask)
        {
            m_PipelineState = m_PipelineStateTask.get();
        }

        Rhi::CommandList CommandList(RhiDevice);

        CommandList.SetPipelineLayout(*m_PipelineState->GetLayout());
        CommandList.SetConstants(0, Math::Colors::Red);

        CommandList.SetPipelineState(*m_PipelineState);

        Rhi::AttachmentsDesc Attachments{};
        Rhi::ResourceView    RenderTargets[]{
            RhiDevice.GetBackbuffer().View
        };
        CommandList.BeginRendering(RenderTargets);

        auto [Viewports, Scissors] = GetViewportsAndScissors(RhiDevice);
        CommandList.SetViewports(Viewports);
        CommandList.SetScissorRects(Scissors);
        CommandList.Draw(Rhi::DrawDesc{ .vertexNum = 3 });
        CommandList.EndRendering();
    }

private:
    [[nodiscard]] ViewportScissor GetViewportsAndScissors(
        Rhi::Device& RhiDevice)
    {
        auto& BackbufferDesc = RhiDevice.GetBackbuffer().Resource.GetDesc(RhiDevice);

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

    [[nodiscard]] Co::result<std::vector<Rhi::ShaderBytecode>>
    LoadShaders(
        Co::executor_tag,
        Co::thread_pool_executor& Executor,
        Rhi::Device&              RhiDevice) const
    {
        std::vector<Rhi::ShaderBytecode> Shaders;
        Shaders.reserve(2);

        constexpr const char* SourceCode = R"(
        struct VSOutput
        {
            float4 pos : SV_POSITION;
            float4 color : COLOR;
        };
        VSOutput VS_Main(uint vertexId : SV_VertexID)
        {
            VSOutput output;

            if (vertexId == 0)
                output.pos = float4(0.0, 0.5, 0.5, 1.0);
            else if (vertexId == 2)
                output.pos = float4(0.5, -0.5, 0.5, 1.0);
            else if (vertexId == 1)
                output.pos = float4(-0.5, -0.5, 0.5, 1.0);

            output.color = clamp(output.pos, 0, 1);
            return output;
        }
        float4 PS_Main(VSOutput ps) : SV_TARGET
        {
	        return ps.color;
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
        Rhi::PushConstantDesc Constants[]{
            { .registerIndex = 0, .size = sizeof(float) * 4, .shaderStages = Rhi::ShaderType::FRAGMENT_SHADER }
        };

        Rhi::PipelineLayoutDesc Desc{
            .pushConstants   = Constants,
            .pushConstantNum = Rhi::Count32(Constants),
            .shaderStages    = Rhi::ShaderType::VERTEX_SHADER | Rhi::ShaderType::FRAGMENT_SHADER
        };
        co_return co_await RhiDevice.CreatePipelineLayout({}, Executor, Desc);
    }

    [[nodiscard]] Co::result<Ptr<Rhi::PipelineState>> CreateBasicPipeline(
        Co::runtime& Coroutine,
        Rhi::Device& RhiDevice)
    {
        auto& Executor = *Coroutine.thread_pool_executor();

        auto ShaderTask = LoadShaders({}, Executor, RhiDevice);
        auto LayoutTask = LoadLayout({}, Executor, RhiDevice);

        Rhi::RenderTargetDesc RenderTargets[]{
            { RhiDevice.GetBackbuffer().Resource.GetDesc(RhiDevice).format }
        };

        Rhi::VertexStreamDesc VertexStreams[]{
            { .stride      = sizeof(float) * 2,
              .bindingSlot = 0,
              .stepRate    = Rhi::VertexStreamStepRate::PER_VERTEX }
        };

        Rhi::VertexAttributeDesc VertexAttributes[]{
            { .d3d{ "POSITION", 0 },
              .vk{ 0 },
              .offset      = 0,
              .format      = Rhi::ResourceFormat::R32_SFLOAT,
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
            //.VertexInput = &VertexInput
        };

        Desc.Layout = co_await LayoutTask;

        auto Shaders     = co_await ShaderTask;
        auto ShaderDescs = Shaders |
                           std::views::transform([](const auto& Shader)
                                                 { return Shader.GetDesc(); }) |
                           std::ranges::to<std::vector>();

        Desc.Shaders = ShaderDescs;

        co_return co_await RhiDevice.CreatePipelineState({}, Executor, Desc);
    }

private:
    Co::result<Ptr<Rhi::PipelineState>> m_PipelineStateTask;
    Ptr<Rhi::PipelineState>             m_PipelineState;
};

AME_MAIN(Argc, Argv)
{
    Log::Logger::Register(Log::Names::Engine, "Engine.log");
    Log::Logger::Register(Log::Names::Rhi, "Engine.log");

    WindowApplication<TriangleSampleEngine>::Builder()
        .Title("Simple Window")
        .RendererBackend(Rhi::DeviceType::DirectX12)
        .Build()
        .Run();
}
