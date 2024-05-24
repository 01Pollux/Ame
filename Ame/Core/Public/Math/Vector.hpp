#pragma once

#include <glm/gtc/quaternion.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

namespace Ame::Math
{
    using Vector2  = glm::vec2;
    using Vector2I = glm::ivec2;
    using Vector2U = glm::uvec2;
    using Vector2B = glm::bvec2;

    using Vector3  = glm::vec3;
    using Vector3I = glm::ivec3;
    using Vector3U = glm::uvec3;
    using Vector3B = glm::bvec3;

    using Vector4  = glm::vec4;
    using Vector4I = glm::ivec4;
    using Vector4U = glm::uvec4;
    using Vector4B = glm::bvec4;

    using Quaternion = glm::quat;
} // namespace Ame::Math

namespace Ame::Concepts
{
    template<typename Ty>
    concept VectorType = std::is_same_v<Ty, Math::Vector2> ||
                         std::is_same_v<Ty, Math::Vector2I> ||
                         std::is_same_v<Ty, Math::Vector2U> ||
                         std::is_same_v<Ty, Math::Vector3> ||
                         std::is_same_v<Ty, Math::Vector3I> ||
                         std::is_same_v<Ty, Math::Vector3U> ||
                         std::is_same_v<Ty, Math::Vector4> ||
                         std::is_same_v<Ty, Math::Vector4I> ||
                         std::is_same_v<Ty, Math::Vector4U>;

    template<typename Ty>
    concept QuaternionType = std::is_same_v<Ty, Math::Quaternion>;
} // namespace Ame::Concepts

namespace Ame::Math::Vec
{
    namespace Impl
    {
        template<Concepts::VectorType Ty>
        [[nodiscard]] static constexpr Ty MakeVec(
            int x,
            int y)
        {
            Ty vec;
            vec.x = static_cast<Ty::value_type>(x);
            vec.y = static_cast<Ty::value_type>(y);
            return vec;
        }
    } // namespace Impl

    //

    template<Concepts::VectorType Ty>
    static constexpr Ty c_One{ 1 };

    template<Concepts::VectorType Ty>
    static constexpr Ty c_Zero{ 0 };

    template<Concepts::VectorType Ty>
    static constexpr Ty c_Up = Impl::MakeVec<Ty>(0, 1);

    template<Concepts::VectorType Ty>
    static constexpr Ty c_Down = Impl::MakeVec<Ty>(0, -1);

    template<Concepts::VectorType Ty>
    static constexpr Ty c_Left = Impl::MakeVec<Ty>(-1, 0);

    template<Concepts::VectorType Ty>
    static constexpr Ty c_Right = Impl::MakeVec<Ty>(1, 0);

    template<Concepts::VectorType Ty>
    static constexpr Ty c_Forward{ 0, 0, 1 };

    template<Concepts::VectorType Ty>
    static constexpr Ty c_Backward{ 0, 0, -1 };

    //

    template<Concepts::QuaternionType Ty>
    inline constexpr Ty c_Identity{ 1, 0, 0, 0 };
} // namespace Ame::Math::Vec

namespace boost::serialization
{
    template<typename ArchiveTy, Ame::Concepts::VectorType Ty>
    void Serialize(
        ArchiveTy& archive,
        Ty&        vec,
        uint32_t)
    {
        for (typename Ty::length_type i = 0; i < vec.length(); i++)
        {
            archive& vec[i];
        }
    }
} // namespace boost::serialization