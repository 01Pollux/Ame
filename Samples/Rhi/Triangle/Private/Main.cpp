
#include <Framework/EntryPoint.hpp>
#include <Framework/Window.hpp>

#include <Rhi/Shader.hpp>

#include <Log/Wrapper.hpp>

using namespace Ame;

class TriangleSampleEngine : public BaseEngine
{
protected:
    void Initialize() override
    {
        BaseEngine::Initialize();

        Log::Engine().Trace("Initializing Sample...");

        printf("Main thread: %d\n", std::this_thread::get_id());
        LoadPipeline(
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

    Co::result<void> LoadPipeline(
        Co::runtime& Coroutine,
        Rhi::Device& RhiDevice)
    {
        auto Shaders = co_await LoadShaders({}, *Coroutine.background_executor(), RhiDevice);

        printf("Shaders: %zu\n", Shaders.size());
        printf("[0] Size: %zu\n", Shaders[0].GetSize());
        printf("[1] Size: %zu\n", Shaders[1].GetSize());

        co_return;
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
