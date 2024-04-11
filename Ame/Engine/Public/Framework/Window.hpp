#pragma once

#include <Framework/Framework.hpp>
#include <Rhi/Subsystem/Device.hpp>
#include <Rhi/Device/CreateDesc.hpp>

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
            const Rhi::DeviceCreateDesc& RhiDesc,
            Ptr<Co::runtime>             Runtime,
            ArgsTy&&... Args) :
            m_Engine(std::forward<ArgsTy>(Args)...)
        {
            m_Engine.RegisterSubsystem<Rhi::DeviceSubsystem>(RhiDesc);
            m_Engine.RegisterSubsystem<CoroutineSubsystem>(std::move(Runtime));
        }

    private:
        EngineType m_Engine;
    };

    template<typename EngineType>
    struct WindowApplication<EngineType>::Builder
    {
    public:
        auto& Title(
            const char* Title)
        {
            TryCreateWindowDesc();
            m_RhiDesc.Window->Window.Title = Title;
            return *this;
        }

        auto& Size(
            const Math::Vector2& Size)
        {
            TryCreateWindowDesc();
            m_RhiDesc.Window->Window.Size = Size;
            return *this;
        }

        auto& CustomTitleBar(
            bool Enable = true)
        {
            TryCreateWindowDesc();
            m_RhiDesc.Window->Window.CustomTitleBar = Enable;
            return *this;
        }

        auto& StartInMiddle(
            bool Enable = true)
        {
            TryCreateWindowDesc();
            m_RhiDesc.Window->Window.StartInMiddle = Enable;
            return *this;
        }

        auto& Fullscreen(
            bool Enable = true)
        {
            TryCreateWindowDesc();
            m_RhiDesc.Window->Window.FullScreen = Enable;
            return *this;
        }

        auto& Maximized(
            bool Enable = true)
        {
            TryCreateWindowDesc();
            m_RhiDesc.Window->Window.Maximized = Enable;
            return *this;
        }

        auto& NoResize(
            bool Enable = true)
        {
            TryCreateWindowDesc();
            m_RhiDesc.Window->Window.NoResize = Enable;
            return *this;
        }

        //

        auto& SwapChainFormat(
            Rhi::SwapChainFormat Format)
        {
            TryCreateWindowDesc();
            m_RhiDesc.Window->Format = Format;
            return *this;
        }

        auto& BackbufferCount(
            uint32_t Count)
        {
            TryCreateWindowDesc();
            m_RhiDesc.Window->BackbufferCount = Count;
            return *this;
        }

        //

        auto& Adapter(
            const Rhi::AdapterDesc& Adapter)
        {
            m_RhiDesc.Adapter = Adapter;
            return *this;
        }

        auto& InstanceExtension(
            const char* Extension)
        {
            m_RequiredInstanceExtensions.push_back(Extension);
            return *this;
        }

        auto& DeviceExtension(
            const char* Extension)
        {
            m_RequiredDeviceExtensions.push_back(Extension);
            return *this;
        }

        auto& FramesInFlight(
            uint32_t FramesInFlight)
        {
            m_RhiDesc.FramesInFlight = FramesInFlight;
            return *this;
        }

        auto& RendererBackend(
            Rhi::DeviceType Backend)
        {
            m_RhiDesc.Type = Backend;
            return *this;
        }

        auto& RayTracing(
            Rhi::DeviceFeatureType Feature = Rhi::DeviceFeatureType::Optional)
        {
            m_RhiDesc.RayTracingFeatures = Feature;
            return *this;
        }

        auto& MeshShader(
            Rhi::DeviceFeatureType Feature = Rhi::DeviceFeatureType::Optional)
        {
            m_RhiDesc.MeshShaderFeatures = Feature;
            return *this;
        }

        auto& ValidationLayer(
            bool Enable = true)
        {
            m_RhiDesc.EnableApiValidationLayer = Enable;
            return *this;
        }

        auto& VSync(
            bool Enable = true)
        {
            m_RhiDesc.EnableVSync = Enable;
            return *this;
        }

    public:
        auto& Runtime(
            Ptr<Co::runtime> Runtime)
        {
            m_Runtime = std::move(Runtime);
            return *this;
        }

    public:
        template<typename... ArgsTy>
        [[nodiscard]] WindowApplication Build(
            ArgsTy&&... Args)
        {
            if (!m_RhiDesc.Adapter)
            {
                m_RhiDesc.SetFirstAdapter();
            }
            if (!m_Runtime)
            {
                m_Runtime = std::make_shared<Co::runtime>();
            }

            m_RhiDesc.RequiredInstanceExtensions = m_RequiredInstanceExtensions;
            m_RhiDesc.RequiredDeviceExtensions   = m_RequiredDeviceExtensions;

            return { m_RhiDesc, std::move(m_Runtime), std::forward<ArgsTy>(Args)... };
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
        Ptr<Co::runtime>         m_Runtime;
        std::vector<const char*> m_RequiredInstanceExtensions;
        std::vector<const char*> m_RequiredDeviceExtensions;
    };
} // namespace Ame::Framework
