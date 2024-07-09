#include <Framework/EntryPoint.hpp>
#include <Framework/Framework.hpp>

#include <Subsystem/Core/EngineTick.hpp>
#include <Subsystem/Core/EngineFrame.hpp>
#include <Subsystem/Core/FrameTimer.hpp>
#include <Subsystem/Rhi/Device.hpp>

#include <Math/Colors.hpp>
#include <Rhi/Device/CreateDesc.hpp>
#include <Window/Window.hpp>

#include <DiligentCore/Graphics/GraphicsTools/interface/MapHelper.hpp>
#include <DiligentCore/Graphics/GraphicsTools/interface/GraphicsUtilities.h>

#include <Log/Wrapper.hpp>

using namespace Ame;

//

static constexpr StringView s_TriangleShader = R"(
struct PSInput
{
	float4 Position : SV_POSITION;
	float4 Color    : COLOR;
};

#ifdef VERTEX_SHADER
cbuffer Constants : register(b0)
{
	float time;
};

PSInput VS_Main(in uint vertexId: SV_VertexID)
{
    float4 positions[3];
    positions[0] = float4(-0.5, -0.5, 0.0, 1.0);
    positions[1] = float4( 0.0, +0.5, 0.0, 1.0);
    positions[2] = float4(+0.5, -0.5, 0.0, 1.0);

    float3 colors[3];
    colors[0] = float3(1.0, 0.0, 0.0); // red
    colors[1] = float3(0.0, 1.0, 0.0); // green
    colors[2] = float3(0.0, 0.0, 1.0); // blue

	PSInput output;
	output.Position = positions[vertexId];
    output.Color = float4(colors[vertexId], 1.f);

    // rotate color based on time
	float angle = time * 3.14159265359;
	float s = sin(angle);
	float c = cos(angle);
	float3x3 rotation = float3x3(
		c, -s, 0,
		s, c, 0,
		0, 0, 1
	);
	output.Color.rgb = mul(rotation, output.Color.rgb);

	return output;
}
#endif

#ifdef PIXEL_SHADER
float4 PS_Main(PSInput input) : SV_TARGET
{
	return input.Color;
}
#endif
)";

//

struct WindowProcessor : public IEngineTick
{
public:
    struct Subsystem;

public:
    WindowProcessor(
        Co::runtime&    runtime,
        EngineFrame&    engineFrame,
        FrameTimer&     frameTimer,
        Rhi::RhiDevice& rhiDevice) :
        m_Runtime(runtime),
        m_EngineFrame(engineFrame),
        m_FrameTimer(frameTimer),
        m_RhiDevice(rhiDevice)
    {
        Initialize();
    }

protected:
    Co::result<void> Tick(
        Co::runtime& runtime)
    {
        Render();
        bool isRunning = m_RhiDevice.get().ProcessTasks();

        m_RhiDevice.get().AdvanceFrame();
        if (!isRunning)
        {
            m_EngineFrame.get().Stop();
        }

        co_return;
    }

private:
    void Render()
    {
        if (!m_PipelineState)
        {
            return;
        }

        auto swapchain     = m_RhiDevice.get().GetSwapchain();
        auto deviceContext = m_RhiDevice.get().GetImmediateContext();

        constexpr auto clearColor = Colors::c_GhostWhite;
        auto           rtv        = swapchain->GetCurrentBackBufferRTV();

        {
            Dg::MapHelper<float> timeMap(deviceContext, m_Buffer, Dg::MAP_WRITE, Dg::MAP_FLAG_DISCARD);
            *timeMap = static_cast<float>(m_FrameTimer.get().GetGameTime());
        }

        deviceContext->SetRenderTargets(1, &rtv, nullptr, Dg::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);
        deviceContext->ClearRenderTarget(rtv, clearColor.data(), Dg::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);

        deviceContext->SetPipelineState(m_PipelineState);
        deviceContext->CommitShaderResources(m_SRB, Dg::RESOURCE_STATE_TRANSITION_MODE_TRANSITION);

        Dg::DrawAttribs drawAttrs{ 3, Dg::DRAW_FLAG_VERIFY_ALL };
        deviceContext->Draw(drawAttrs);
    }

private:
    Co::result<Dg::Ptr<Dg::IShader>> CreateShader(
        const char*     entryPoint,
        Dg::SHADER_TYPE type)
    {
        Dg::ShaderCreateInfo shaderCreateDesc{
            s_TriangleShader.data(),
            s_TriangleShader.size(),
            entryPoint,
            Dg::SHADER_SOURCE_LANGUAGE_HLSL,
            Dg::ShaderDesc{ "Triangle shader", type }
        };

        Dg::Ptr<Dg::IShader> shader;
        Dg::IDataBlob**      outputDebugPtr = nullptr;

        auto renderDevice = m_RhiDevice.get().GetDevice();
        renderDevice->CreateShader(shaderCreateDesc, &shader);

        co_return shader;
    }

    Co::result<Dg::Ptr<Dg::IBuffer>> CreateBuffer()
    {
        Dg::Ptr<Dg::IBuffer> buffer;

        auto renderDevice = m_RhiDevice.get().GetDevice();
        Dg::CreateUniformBuffer(renderDevice, sizeof(float), "Buffer", &buffer);

        co_return buffer;
    }

    Co::null_result Initialize()
    {
        auto swapchain    = m_RhiDevice.get().GetSwapchain();
        auto renderDevice = m_RhiDevice.get().GetDevice();

        auto vertexShaderTask = CreateShader("VS_Main", Dg::SHADER_TYPE_VERTEX);
        auto pixelShaderTask  = CreateShader("PS_Main", Dg::SHADER_TYPE_PIXEL);

        Dg::GraphicsPipelineStateCreateInfo pipelineDesc;

        pipelineDesc.PSODesc.Name         = "Triangle PSO";
        pipelineDesc.PSODesc.PipelineType = Dg::PIPELINE_TYPE_GRAPHICS;

        pipelineDesc.GraphicsPipeline.NumRenderTargets  = 1;
        pipelineDesc.GraphicsPipeline.RTVFormats[0]     = swapchain->GetDesc().ColorBufferFormat;
        pipelineDesc.GraphicsPipeline.PrimitiveTopology = Dg::PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

        std::tie(pixelShaderTask, vertexShaderTask) =
            (co_await Co::when_all(
                m_Runtime.get().background_executor(),
                std::move(pixelShaderTask), std::move(vertexShaderTask)));

        auto vertexShader = co_await vertexShaderTask;
        auto pixelShader  = co_await pixelShaderTask;

        pipelineDesc.pVS = vertexShader;
        pipelineDesc.pPS = pixelShader;

        Dg::Ptr<Dg::IPipelineState> pipelineState;
        renderDevice->CreatePipelineState(pipelineDesc, &pipelineState);

        m_Buffer = co_await CreateBuffer();
        pipelineState->GetStaticVariableByName(Dg::SHADER_TYPE_VERTEX, "Constants")->Set(m_Buffer);
        pipelineState->CreateShaderResourceBinding(&m_SRB, true);

        m_PipelineState = std::move(pipelineState);
        co_return;
    }

private:
    Ref<Co::runtime>    m_Runtime;
    Ref<EngineFrame>    m_EngineFrame;
    Ref<FrameTimer>     m_FrameTimer;
    Ref<Rhi::RhiDevice> m_RhiDevice;

    Dg::Ptr<Dg::IPipelineState>         m_PipelineState;
    Dg::Ptr<Dg::IShaderResourceBinding> m_SRB;
    Dg::Ptr<Dg::IBuffer>                m_Buffer;
};

struct WindowProcessor::Subsystem : public SingleSubsystem<
                                        WindowProcessor,
                                        Dependency<
                                            CoroutineSubsystem,
                                            EngineFrameSubsystem,
                                            FrameTimerSubsystem,
                                            Rhi::RhiDeviceSubsystem>>,
                                    kgr::overrides<EngineTickSubsystem>
{
};

AME_MAIN(Argc, Argv)
{
    Log::Logger::Register(Log::Names::c_Engine, "Engine.log");
    Log::Logger::Register(Log::Names::c_Rhi, "Engine.log");
    Log::Logger::Register(Log::Names::c_Client, "Engine.log");
    Log::Logger::Register(Log::Names::c_Window, "Window.log");

    auto getDeviceCreateDesc = []() -> Rhi::DeviceCreateDesc
    {
        Windowing::WindowDesc windowDesc{
            .Title = "Triangle"
        };

        return Rhi::DeviceCreateDesc{
            .Types{
                Rhi::DeviceCreateDescD3D12{},
                Rhi::DeviceCreateDescVulkan{},
                Rhi::DeviceCreateDescD3D11{},
                Rhi::DeviceCreateDescGL{} },
            .Surface = Rhi::RenderSurfaceDesc{
                std::make_shared<Windowing::Window>(windowDesc) }
        };
    };

    Bootstrap::Application()
        .With<Rhi::RhiDeviceSubsystem>(getDeviceCreateDesc())
        .With<WindowProcessor::Subsystem>()
        .Build()
        .Run()
        .wait();
}
