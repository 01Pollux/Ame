
#include <Framework/EntryPoint.hpp>
#include <Framework/Window.hpp>

#include <Rhi/Shader.hpp>
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

        auto Pipeline = CreateBasicPipeline(
                            *GetSubsystem<CoroutineSubsystem>(),
                            GetSubsystem<Rhi::DeviceSubsystem>())
                            .get();
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

			VertexOutput VS_Main(VertexInput input)
			{
				VertexOutput output;
				output.Position = float4(input.Position, 0.0f, 1.0f);
				return output;
			}

			float4 PS_Main(VertexOutput input) : SV_TARGET
			{
				return float4(1.0f, 0.0f, 0.0f, 1.0f);
			}
		)";

        auto VertexShader = Rhi::ShaderBytecode::Compile({}, Executor, RhiDevice.GetGraphicsAPI(), SourceCode, { .Stage = Rhi::ShaderType::VERTEX_SHADER });
        auto PixelShader  = Rhi::ShaderBytecode::Compile({}, Executor, RhiDevice.GetGraphicsAPI(), SourceCode, { .Stage = Rhi::ShaderType::FRAGMENT_SHADER });

        Shaders.emplace_back(co_await VertexShader);
        Shaders.emplace_back(co_await PixelShader);

        co_return Shaders;
    }

    [[nodiscard]] Co::result<Ptr<Rhi::PipelineLayout>> LoadLayout(
        Co::executor_tag,
        Co::thread_pool_executor& Executor,
        Rhi::Device&              RhiDevice) const
    {
        Rhi::PipelineLayoutDesc Desc{};
        co_return RhiDevice.CreatePipelineLayout({}, Executor, Desc);
    }

    [[nodiscard]] Co::result<Ptr<Rhi::PipelineState>> CreateBasicPipeline(
        Co::runtime& Coroutine,
        Rhi::Device& RhiDevice)
    {
        auto& Executor = *Coroutine.thread_pool_executor();

        auto Shaders = LoadShaders({}, Executor, RhiDevice);
        auto Layout  = LoadLayout({}, Executor, RhiDevice);

        Rhi::RenderTargetDesc RenderTargets[]{
            { RhiDevice.GetBackbuffer().Resource.GetDesc(RhiDevice).format }
        };

        Rhi::GraphicsPipelineDesc Desc{
            .InputAssembly{
                Rhi::TopologyType::TRIANGLE_LIST },
            .OutputMerger{
                RenderTargets }
        };

        Desc.Layout = co_await Layout;

        auto ShaderDescs = co_await Shaders |
                           std::views::transform([](const auto& Shader)
                                                 { return Shader.GetDesc(); }) |
                           std::ranges::to<std::vector>();

        Desc.Shaders = ShaderDescs;

        co_return RhiDevice.CreatePipelineState({}, Executor, Desc);
    }
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
