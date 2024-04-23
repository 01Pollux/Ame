#pragma once

#include <Framework/Framework.hpp>
#include <Rhi/Subsystem/Device.hpp>
#include <Rhi/Device/CreateDesc.hpp>
#include <Gfx/Subsystem/Renderer.hpp>

namespace Ame::Framework
{
  static  void foo()
    {
        constexpr auto px = kgr::detail::is_emplace_valid<Rhi::DeviceSubsystem>::value;
        constexpr auto pxs = kgr::detail::is_emplace_valid<Gfx::RendererSubsystem>::value;

        using T           = Gfx::RendererSubsystem;
        constexpr auto a  = kgr::detail::is_single<T>::value;
        constexpr auto b  = kgr::detail::is_construction_valid<T>::value;
        constexpr auto ba = kgr::detail::service_check<T>::value;
        constexpr auto baa = kgr::detail::shallow_service_check<T>::value;
        constexpr auto bab = kgr::detail::dependency_check<T>::value;
        constexpr auto bb = kgr::detail::is_autocall_valid<T>::value;
        constexpr auto bc = kgr::detail::dependency_trait<kgr::detail::is_autocall_valid, T>::value;
        constexpr auto c  = kgr::detail::is_construct_function_callable<T>::value;
        constexpr auto d  = kgr::detail::is_service_constructible<T>::value;
    }

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
            m_Engine.RegisterSubsystem<Gfx::RendererSubsystem>();
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
