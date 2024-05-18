#pragma once

#include <glm/mat3x3.hpp>
#include <glm/mat4x4.hpp>

namespace Ame::Math
{
    using Matrix2x2  = glm::mat2x2;
    using Matrix2x2I = glm::tmat2x2<int32_t>;
    using Matrix2x2U = glm::tmat2x2<uint32_t>;

    using Matrix3x3  = glm::mat3x3;
    using Matrix3x3I = glm::tmat3x3<int32_t>;
    using Matrix3x3U = glm::tmat3x3<uint32_t>;

    using Matrix4x4  = glm::mat4x4;
    using Matrix4x4I = glm::tmat4x4<int32_t>;
    using Matrix4x4U = glm::tmat4x4<uint32_t>;
} // namespace Ame::Math

namespace Ame::Concepts
{
    template<typename Ty>
    concept MatrixType = std::is_same_v<Ty, Math::Matrix3x3> ||
                         std::is_same_v<Ty, Math::Matrix3x3I> ||
                         std::is_same_v<Ty, Math::Matrix3x3U> ||
                         std::is_same_v<Ty, Math::Matrix4x4> ||
                         std::is_same_v<Ty, Math::Matrix4x4I> ||
                         std::is_same_v<Ty, Math::Matrix4x4U>;
}

namespace Ame::Math::Mat
{
    template<Concepts::MatrixType Ty>
    inline constexpr Ty Zero{ 0.f };

    template<Concepts::MatrixType Ty>
    inline constexpr Ty Identity{ 1.f };
} // namespace Ame::Math::Mat

namespace boost::serialization
{
    template<typename ArchiveTy, Ame::Concepts::MatrixType Ty>
    void serialize(
        ArchiveTy& Archive,
        Ty&        Mat,
        uint32_t)
    {
        for (typename Ty::length_type i = 0; i < Mat.length(); i++)
        {
            Archive& Mat[i];
        }
    }
} // namespace boost::serialization