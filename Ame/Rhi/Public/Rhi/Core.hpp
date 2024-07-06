#pragma once

#include <span>

#include <Core/Ame.hpp>

#include <DiligentCore/Common/interface/RefCntAutoPtr.hpp>
#include <DiligentCore/Graphics/GraphicsEngine/interface/GraphicsTypes.h>
#include <DiligentCore/Graphics/GraphicsEngine/interface/EngineFactory.h>
#include <DiligentCore/Graphics/GraphicsEngine/interface/RenderDevice.h>
#include <DiligentCore/Graphics/GraphicsEngine/interface/DeviceContext.h>
#include <DiligentCore/Graphics/GraphicsEngine/interface/SwapChain.h>

namespace Diligent
{
    template<typename Ty>
    using Ptr = RefCntAutoPtr<Ty>;

    template<typename Ty>
    using WeakPtr = RefCntWeakPtr<Ty>;
} // namespace Diligent

namespace Ame::Windowing
{
    class Window;
} // namespace Ame::Windowing

namespace Ame::Rhi
{
    namespace Dg = Diligent;

    class DeviceWrapper;
    class RhiDevice;

    //

    template<typename Ty>
    [[nodiscard]] constexpr size_t Count64(
        const Ty& value)
    {
        return std::size(value);
    }

    template<typename Ty>
        requires std::is_standard_layout_v<Ty> && std::is_trivial_v<Ty>
    [[nodiscard]] constexpr size_t Size64()
    {
        return sizeof(Ty);
    }

    template<typename Ty>
        requires std::is_standard_layout_v<Ty> && std::is_trivial_v<Ty>
    [[nodiscard]] constexpr size_t Size64(
        const Ty&)
    {
        return sizeof(Ty);
    }

    //

    template<typename Ty>
    [[nodiscard]] constexpr uint32_t Count32(
        const Ty& value)
    {
        return static_cast<uint32_t>(std::size(value));
    }

    template<typename Ty>
        requires std::is_standard_layout_v<Ty> && std::is_trivial_v<Ty>
    [[nodiscard]] constexpr uint32_t Size32()
    {
        return static_cast<uint32_t>(sizeof(Ty));
    }

    template<typename Ty>
        requires std::is_standard_layout_v<Ty> && std::is_trivial_v<Ty>
    [[nodiscard]] constexpr uint32_t Size32(
        const Ty&)
    {
        return static_cast<uint32_t>(sizeof(Ty));
    }

    //

    template<typename Ty>
    [[nodiscard]] constexpr uint16_t Count16(
        const Ty& value)
    {
        return static_cast<uint16_t>(std::size(value));
    }

    template<typename Ty>
        requires std::is_standard_layout_v<Ty> && std::is_trivial_v<Ty>
    [[nodiscard]] constexpr uint16_t Size16()
    {
        return static_cast<uint16_t>(sizeof(Ty));
    }

    template<typename Ty>
        requires std::is_standard_layout_v<Ty> && std::is_trivial_v<Ty>
    [[nodiscard]] constexpr uint16_t Size16(
        const Ty&)
    {
        return static_cast<uint16_t>(sizeof(Ty));
    }

    //

    template<typename Ty>
    [[nodiscard]] constexpr uint8_t Count8(
        const Ty& value)
    {
        return static_cast<uint8_t>(std::size(value));
    }

    template<typename Ty>
        requires std::is_standard_layout_v<Ty> && std::is_trivial_v<Ty>
    [[nodiscard]] constexpr uint8_t Size8()
    {
        return static_cast<uint8_t>(sizeof(Ty));
    }

    template<typename Ty>
        requires std::is_standard_layout_v<Ty> && std::is_trivial_v<Ty>
    [[nodiscard]] constexpr uint8_t Size8(
        const Ty&)
    {
        return static_cast<uint8_t>(sizeof(Ty));
    }
} // namespace Ame::Rhi