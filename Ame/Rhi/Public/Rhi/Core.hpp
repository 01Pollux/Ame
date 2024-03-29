#pragma once

#include <NRIDescs.h>
#include <Core/Ame.hpp>

namespace Ame::Rhi
{
    class Device;
    class Buffer;
    class Texture;
    class ResourceView;
} // namespace Ame::Rhi

namespace Ame::Concepts
{
    template<typename Ty>
    concept RhiObject = requires(Rhi::Device& RhiDevice, Ty& Object) {
        // has function Release(Device&)
        {
            Object.Release(RhiDevice)
        } -> std::same_as<void>;
        {
            Object.operator bool()
        } -> std::same_as<bool>;
    };

    template<typename Ty>
    concept RhiDeferObject = requires(Rhi::Device& RhiDevice, Ty& Object) {
        {
            Object.DeferRelease(RhiDevice)
        } -> std::same_as<void>;
        {
            Object.operator bool()
        } -> std::same_as<bool>;
    };
} // namespace Ame::Concepts

namespace Ame::Rhi
{
    namespace Impl
    {
        template<Concepts::RhiObject Ty, bool Defer>
        class AutoRelease : public NonCopyable
        {
        public:
            AutoRelease() = default;

            AutoRelease(
                Device& RhiDevice,
                Ty&&    Object) :
                m_Device(&RhiDevice),
                m_Object(std::move(Object))
            {
            }

            AutoRelease(
                AutoRelease&& Other) noexcept :
                m_Device(Other.m_Device),
                m_Object(std::move(Other.m_Object))
            {
            }

            AutoRelease& operator=(
                AutoRelease&& Other) noexcept
            {
                if (this != &Other)
                {
                    if (m_Object)
                    {
                        Release();
                    }
                    m_Device = Other.m_Device;
                    m_Object = std::exchange(Other.m_Object, {});
                }
                return *this;
            }

            ~AutoRelease()
            {
                if (m_Object)
                {
                    Release();
                }
            }

        public:
            Ty& operator*()
            {
                return m_Object;
            }
            Ty* operator->()
            {
                return &m_Object;
            }

            Ty const& operator*() const
            {
                return m_Object;
            }
            Ty const* operator->() const
            {
                return &m_Object;
            }

            Ty& Get()
            {
                return m_Object;
            }

        private:
            void Release()
            {
                if constexpr (Defer)
                {
                    m_Object.DeferRelease(*m_Device);
                }
                else
                {
                    m_Object.Release(*m_Device);
                }
            }

        private:
            Device* m_Device;
            Ty      m_Object;
        };
    } // namespace Impl

    template<Concepts::RhiObject Ty>
    using AutoRelease = Impl::AutoRelease<Ty, false>;
    template<Concepts::RhiDeferObject Ty>
    using DeferRelease = Impl::AutoRelease<Ty, true>;

    enum class GraphicsAPI : uint8_t
    {
        Null,
        DirectX12,
        Vulkan,

        Count,
    };

    enum class DeviceFeatureType : uint8_t
    {
        Disabled,
        Optional,
        Required
    };

    // Color space:
    //  - BT.709 - LDR https://en.wikipedia.org/wiki/Rec._709
    //  - BT.2020 - HDR https://en.wikipedia.org/wiki/Rec._2020
    // Transfer function:
    //  - G10 - linear (gamma 1.0)
    //  - G22 - sRGB (gamma ~2.2)
    //  - G2084 - SMPTE ST.2084 (Perceptual Quantization)
    // Bits per channel:
    //  - 8, 10, 16 (float)
    enum class SwapchainFormat : uint8_t
    {
        BT709_G10_16BIT,
        BT709_G22_8BIT,
        BT709_G22_10BIT,
        BT2020_G2084_10BIT,

        Count,
    };
} // namespace Ame::Rhi