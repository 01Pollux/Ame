#pragma once

#include <span>

#include <Core/Ame.hpp>
#include <NRI/NRIDescs.h>

namespace Ame::Rhi
{
    class Device;
    class DeviceImpl;
    class CommandList;
    class CommandListImpl;
    class PipelineLayout;
    class PipelineState;
    class Buffer;
    class Texture;
    class ResourceView;

    using Mip_t    = uint8_t;
    using Dim_t    = uint16_t;
    using Sample_t = uint8_t;
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
    enum class SwapChainFormat : uint8_t
    {
        BT709_G10_16BIT,
        BT709_G22_8BIT,
        BT709_G22_10BIT,
        BT2020_G2084_10BIT,

        Count,
    };

    //

    template<typename Ty>
    [[nodiscard]] constexpr size_t ByteSizeOf(
        const Ty& Value)
    {
        // if constexpr has begin and end, use them
        if constexpr (requires { Value.begin(); Value.end(); })
        {
            return std::distance(Value.begin(), Value.end()) * sizeof(typename Ty::value_type);
        }
        else
        {
            return sizeof(Value);
        }
    }

    //

    template<typename Ty>
    [[nodiscard]] constexpr size_t Count64(
        const Ty& Value)
    {
        return std::size(Value);
    }

    template<typename Ty>
    [[nodiscard]] constexpr size_t Size64()
    {
        return sizeof(Ty);
    }

    template<typename Ty>
    [[nodiscard]] constexpr size_t Size64(
        const Ty&)
    {
        return sizeof(Ty);
    }

    //

    template<typename Ty>
    [[nodiscard]] constexpr uint32_t Count32(
        const Ty& Value)
    {
        return static_cast<uint32_t>(std::size(Value));
    }

    template<typename Ty>
    [[nodiscard]] constexpr uint32_t Size32()
    {
        return static_cast<uint32_t>(sizeof(Ty));
    }

    template<typename Ty>
    [[nodiscard]] constexpr uint32_t Size32(
        const Ty&)
    {
        return static_cast<uint32_t>(sizeof(Ty));
    }

    //

    template<typename Ty>
    [[nodiscard]] constexpr uint16_t Count16(
        const Ty& Value)
    {
        return static_cast<uint16_t>(std::size(Value));
    }

    template<typename Ty>
    [[nodiscard]] constexpr uint16_t Size16()
    {
        return static_cast<uint16_t>(sizeof(Ty));
    }

    template<typename Ty>
    [[nodiscard]] constexpr uint16_t Size16(
        const Ty&)
    {
        return static_cast<uint16_t>(sizeof(Ty));
    }

    //

    template<typename Ty>
    [[nodiscard]] constexpr uint8_t Count8(
        const Ty& Value)
    {
        return static_cast<uint8_t>(std::size(Value));
    }

    template<typename Ty>
    [[nodiscard]] constexpr uint8_t Size8()
    {
        return static_cast<uint8_t>(sizeof(Ty));
    }

    template<typename Ty>
    [[nodiscard]] constexpr uint8_t Size8(
        const Ty&)
    {
        return static_cast<uint8_t>(sizeof(Ty));
    }
} // namespace Ame::Rhi