#pragma once

#include <Framework/Framework.hpp>
#include <Rhi/Device/CreateDesc.hpp>

#include <Subsystem/Rhi/Device.hpp>

namespace Ame::Framework
{
    template<typename EngineType>
    class WindowApplication
    {
    public:
        struct Builder;

        auto& Run()
        {
            m_Engine.Run();
            return *this;
        }

        auto& Close()
        {
            m_Engine.Close();
            return *this;
        }

    private:
        template<typename... ArgsTy>
        WindowApplication(
            const Rhi::DeviceCreateDesc& rhiDesc,
            const Co::runtime_options&   runtimeOptions,
            ArgsTy&&... args) :
            m_Engine(std::forward<ArgsTy>(args)...)
        {
            m_Engine.RegisterSubsystem<Rhi::DeviceSubsystem>(rhiDesc);
            m_Engine.RegisterSubsystem<CoroutineSubsystem>(runtimeOptions);
        }

    private:
        EngineType m_Engine;
    };

    template<typename EngineType>
    struct WindowApplication<EngineType>::Builder
    {
    public:
        auto& Title(
            const char* title)
        {
            TryCreateWindowDesc();
            m_RhiDesc.Window->Window.Title = title;
            return *this;
        }

        auto& Size(
            const Math::Vector2& size)
        {
            TryCreateWindowDesc();
            m_RhiDesc.Window->Window.Size = size;
            return *this;
        }

        auto& CustomTitleBar(
            bool enable = true)
        {
            TryCreateWindowDesc();
            m_RhiDesc.Window->Window.CustomTitleBar = enable;
            return *this;
        }

        auto& StartInMiddle(
            bool enable = true)
        {
            TryCreateWindowDesc();
            m_RhiDesc.Window->Window.StartInMiddle = enable;
            return *this;
        }

        auto& Fullscreen(
            bool enable = true)
        {
            TryCreateWindowDesc();
            m_RhiDesc.Window->Window.FullScreen = enable;
            return *this;
        }

        auto& Maximized(
            bool enable = true)
        {
            TryCreateWindowDesc();
            m_RhiDesc.Window->Window.Maximized = enable;
            return *this;
        }

        auto& NoResize(
            bool enable = true)
        {
            TryCreateWindowDesc();
            m_RhiDesc.Window->Window.NoResize = enable;
            return *this;
        }

        //

        auto& SwapChainFormat(
            Rhi::SwapChainFormat format)
        {
            TryCreateWindowDesc();
            m_RhiDesc.Window->Format = format;
            return *this;
        }

        auto& BackbufferCount(
            uint32_t count)
        {
            TryCreateWindowDesc();
            m_RhiDesc.Window->BackbufferCount = count;
            return *this;
        }

        //

        auto& Adapter(
            const Rhi::AdapterDesc& adapter)
        {
            m_RhiDesc.Adapter = adapter;
            return *this;
        }

        auto& InstanceExtension(
            const char* extension)
        {
            m_RequiredInstanceExtensions.push_back(extension);
            return *this;
        }

        auto& DeviceExtension(
            const char* extension)
        {
            m_RequiredDeviceExtensions.push_back(extension);
            return *this;
        }

        auto& FramesInFlight(
            uint32_t framesInFlight)
        {
            m_RhiDesc.FramesInFlight = framesInFlight;
            return *this;
        }

        auto& RendererBackend(
            Rhi::DeviceType backend)
        {
            m_RhiDesc.Type = backend;
            return *this;
        }

        auto& RayTracing(
            Rhi::DeviceFeatureType feature = Rhi::DeviceFeatureType::Optional)
        {
            m_RhiDesc.RayTracingFeatures = feature;
            return *this;
        }

        auto& MeshShader(
            Rhi::DeviceFeatureType feature = Rhi::DeviceFeatureType::Optional)
        {
            m_RhiDesc.MeshShaderFeatures = feature;
            return *this;
        }

        auto& ValidationLayer(
            bool enable = true)
        {
            m_RhiDesc.EnableApiValidationLayer = enable;
            return *this;
        }

        auto& VSync(
            bool enable = true)
        {
            m_RhiDesc.EnableVSync = enable;
            return *this;
        }

    public:
        auto& RuntimeOptions()
        {
            return m_RuntimeOptions;
        }

        auto& RuntimeOptions(
            const Co::runtime_options& runtimeOptions)
        {
            m_RuntimeOptions = runtimeOptions;
            return *this;
        }

    public:
        template<typename... ArgsTy>
        [[nodiscard]] WindowApplication Build(
            ArgsTy&&... args)
        {
            if (!m_RhiDesc.Adapter)
            {
                m_RhiDesc.SetFirstAdapter();
            }

            m_RhiDesc.RequiredInstanceExtensions = m_RequiredInstanceExtensions;
            m_RhiDesc.RequiredDeviceExtensions   = m_RequiredDeviceExtensions;

            return { m_RhiDesc, m_RuntimeOptions, std::forward<ArgsTy>(args)... };
        }

    private:
        /// <summary>
        /// Try to create the window desc if it does not exist
        /// </summary>
        void TryCreateWindowDesc()
        {
            if (!m_RhiDesc.Window.has_value()) [[unlikely]]
            {
                m_RhiDesc.Window = Rhi::WindowDesc{};
            }
        }

    private:
        Rhi::DeviceCreateDesc    m_RhiDesc;
        Co::runtime_options      m_RuntimeOptions;
        std::vector<const char*> m_RequiredInstanceExtensions;
        std::vector<const char*> m_RequiredDeviceExtensions;
    };
} // namespace Ame::Framework
