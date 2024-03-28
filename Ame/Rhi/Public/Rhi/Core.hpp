#pragma once

#include <NRIDescs.h>
#include <Core/Ame.hpp>

namespace Ame::Concepts
{
    template<typename Ty>
    concept RhiObject = requires(Ty& Object) {
        {
            Object.Release()
        } -> std::same_as<void>;
    };

    template<typename Ty>
    concept RhiDeferObject = requires(Ty& Object) {
        {
            Object.DeferRelease()
        } -> std::same_as<void>;
    };
} // namespace Ame::Concepts

namespace Ame::Rhi
{
    template<Concepts::RhiObject Ty>
    class AutoRelease : public NonCopyable
    {
    public:
        AutoRelease() = default;

        AutoRelease(Ty&& Object) :
            m_Object(std::move(Object)), m_IsReleased(false)
        {
        }

        AutoRelease(AutoRelease&& Other) noexcept :
            m_Object(std::move(Other.m_Object)), m_IsReleased(Other.m_IsReleased)
        {
            Other.m_IsReleased = true;
        }

        AutoRelease& operator=(AutoRelease&& Other) noexcept
        {
            if (this != &Other)
            {
                if (m_Object)
                {
                    m_Object.Release();
                }
                m_Object           = std::exchange(Other.m_Object, {});
                m_IsReleased       = Other.m_IsReleased;
                Other.m_IsReleased = true;
            }
            return *this;
        }

        ~AutoRelease()
        {
            if (!m_IsReleased)
            {
                m_Object.Release();
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
        Ty   m_Object;
        bool m_IsReleased = true;
    };

    template<Concepts::RhiDeferObject Ty>
    class DeferRelease : public NonCopyable
    {
    public:
        DeferRelease() = default;

        DeferRelease(Ty&& Object) :
            m_Object(std::move(Object))
        {
        }

        DeferRelease(DeferRelease&& Other) noexcept :
            m_Object(std::move(Other.m_Object))
        {
        }

        DeferRelease& operator=(DeferRelease&& Other) noexcept
        {
            if (this != &Other)
            {
                if (m_Object)
                {
                    m_Object.DeferRelease();
                }
                m_Object           = std::exchange(Other.m_Object, {});
                m_IsReleased       = Other.m_IsReleased;
                Other.m_IsReleased = true;
            }
        }

        ~DeferRelease()
        {
            if (!m_IsReleased)
            {
                m_Object.DeferRelease();
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
        Ty   m_Object;
        bool m_IsReleased = true;
    };

    enum class GraphicsAPI : uint8_t
    {
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