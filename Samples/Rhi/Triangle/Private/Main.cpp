
#include <Framework/EntryPoint.hpp>
#include <Framework/Window.hpp>

#include <Rhi/Shader.hpp>
#include <Rhi/CommandList.hpp>
#include <Rhi/PipelineState.hpp>
#include <ranges>

#include <Log/Wrapper.hpp>

using namespace Ame;

class TriangleSampleEngine : public BaseEngine
{
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

private:
    void Render(
        Rhi::Device& RhiDevice)
    {
        if (m_PipelineStateTask)
        {
            m_PipelineState = m_PipelineStateTask.get();
        }

        Rhi::CommandList& CommandList = RhiDevice.GetCommandList();

        CommandList.SetPipelineLayout(*m_PipelineState->GetLayout());
        CommandList.SetConstants(Math::Colors::Red);

        CommandList.SetPipelineState(*m_PipelineState);

        Rhi::AttachmentsDesc Attachments{};
        Rhi::ResourceView    RenderTargets[]{
            RhiDevice.GetBackbuffer().View
        };
        CommandList.BeginRendering(RenderTargets);
        CommandList.Draw(Rhi::DrawDesc{ .vertexNum = 3 });
        CommandList.EndRendering();
    }

private:
    [[nodiscard]] Co::result<std::vector<Rhi::ShaderBytecode>> LoadShaders(
        Co::executor_tag,
        Co::thread_pool_executor& Executor,
        Rhi::Device&              RhiDevice) const
    {
        std::vector<Rhi::ShaderBytecode> Shaders;
        Shaders.reserve(2);

        constexpr const char* SourceCode = R"(
			struct VertexInput
			{
				float2 Position : POSITION;
			};

			struct VertexOutput
			{
				float4 Position : SV_POSITION;
			};

            struct ConstantData
			{
                float4 Color;
			};

#ifdef AME_SHADER_COMPILER_SPIRV
            [[vk::push_constant]] ConstantData Constants : register(b0, space0);
#else
            ConstantBuffer<ConstantData> Constants : register(b0, space0);
#endif

			VertexOutput VS_Main(VertexInput input)
			{
				VertexOutput output;
				output.Position = float4(input.Position, 0.0f, 1.0f);
				return output;
			}

			float4 PS_Main(VertexOutput input) : SV_TARGET
			{
				return Constants.Color;
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
        .Build()
        .Run();
}
