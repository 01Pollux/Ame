#pragma once

#include <DirectXMath.h>
#include <numbers>
#include <Core/Ame.hpp>

namespace Ame::Math::Util
{
    using namespace DirectX;

    template<typename Ty>
    [[nodiscard]] constexpr Ty DegToRad(Ty deg) noexcept
    {
        return deg * (std::numbers::pi_v<float> / 180.f);
    }

    template<typename Ty>
    [[nodiscard]] constexpr Ty RadToDeg(Ty rad) noexcept
    {
        return rad * (180.f / std::numbers::pi_v<float>);
    }
} // namespace Ame::Math::Util
