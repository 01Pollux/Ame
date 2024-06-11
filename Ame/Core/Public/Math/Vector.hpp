#pragma once

#include <Math/Vector.Impl.hpp>

namespace Ame::Math
{
    class Matrix4x4;
    class Matrix3x3;

    //

    class Vector2 : public Impl::MVector<float, 2>
    {
    public:
        using MVector::MVector;
        using Constants = Impl::VectorConstants<Vector2>;

        constexpr Vector2(float x, float y) noexcept :
            MVector({ x, y })
        {
        }

        Vector2(Util::FXMVECTOR vec);
        Vector2(Util::XMFLOAT2 vec);

        operator Util::XMVECTOR() const;
        operator Util::XMFLOAT2() const;

    public:
        AME_VECTOR_IMPL_MATHVEC_DECLARATIONS(Vector2);

    public:
        AME_VECTOR_IMPL_ACCESSOR(0, x);
        AME_VECTOR_IMPL_ACCESSOR(1, y);

    public:
        [[nodiscard]] float Cross(const Vector2& other) const noexcept;
    };

    class Vector2I : public Impl::MVector<int, 2>
    {
    public:
        using MVector::MVector;
        using Constants = Impl::VectorConstants<Vector2I>;

        constexpr Vector2I(int x, int y) noexcept :
            MVector({ x, y })
        {
        }

        Vector2I(Util::FXMVECTOR vec);
        Vector2I(Util::XMINT2 vec);

        operator Util::XMVECTOR() const;
        operator Util::XMINT2() const;

    public:
        AME_VECTOR_IMPL_MATHVEC_DECLARATIONS(Vector2I);

    public:
        AME_VECTOR_IMPL_ACCESSOR(0, x);
        AME_VECTOR_IMPL_ACCESSOR(1, y);
    };

    class Vector2U : public Impl::MVector<uint32_t, 2>
    {
    public:
        using MVector::MVector;
        using Constants = Impl::VectorConstants<Vector2U>;

        constexpr Vector2U(uint32_t x, uint32_t y) noexcept :
            MVector({ x, y })
        {
        }

        Vector2U(Util::FXMVECTOR vec);
        Vector2U(Util::XMUINT2 vec);

        operator Util::XMVECTOR() const;
        operator Util::XMUINT2() const;

    public:
        AME_VECTOR_IMPL_MATHVEC_DECLARATIONS(Vector2I);

    public:
        AME_VECTOR_IMPL_ACCESSOR(0, x);
        AME_VECTOR_IMPL_ACCESSOR(1, y);
    };

    //

    class Vector3 : public Impl::MVector<float, 3>
    {
    public:
        using MVector::MVector;
        using Constants = Impl::VectorConstants<Vector3>;

        constexpr Vector3(float x, float y, float z) noexcept :
            MVector({ x, y, z })
        {
        }

        Vector3(Util::FXMVECTOR vec);
        Vector3(Util::XMFLOAT3 vec);

        operator Util::XMVECTOR() const;
        operator Util::XMFLOAT3() const;

    public:
        AME_VECTOR_IMPL_MATHVEC_DECLARATIONS(Vector3);

    public:
        AME_VECTOR_IMPL_ACCESSOR(0, x);
        AME_VECTOR_IMPL_ACCESSOR(1, y);
        AME_VECTOR_IMPL_ACCESSOR(2, z);

    public:
        AME_VECTOR_IMPL_ACCESSOR(0, r);
        AME_VECTOR_IMPL_ACCESSOR(1, g);
        AME_VECTOR_IMPL_ACCESSOR(2, b);

    public:
        [[nodiscard]] Vector3 Cross(const Vector3& Other) const noexcept;
    };

    class Vector3I : public Impl::MVector<int, 3>
    {
    public:
        using MVector::MVector;
        using Constants = Impl::VectorConstants<Vector3I>;

        constexpr Vector3I(int x, int y, int z) noexcept :
            MVector({ x, y, z })
        {
        }

        Vector3I(Util::FXMVECTOR vec);
        Vector3I(Util::XMINT3 vec);

        operator Util::XMVECTOR() const;
        operator Util::XMINT3() const;

    public:
        AME_VECTOR_IMPL_MATHVEC_DECLARATIONS(Vector3I);

    public:
        AME_VECTOR_IMPL_ACCESSOR(0, x);
        AME_VECTOR_IMPL_ACCESSOR(1, y);
        AME_VECTOR_IMPL_ACCESSOR(2, z);
    };

    class Vector3U : public Impl::MVector<uint32_t, 3>
    {
    public:
        using MVector::MVector;
        using Constants = Impl::VectorConstants<Vector3U>;

        constexpr Vector3U(uint32_t x, uint32_t y, uint32_t z) noexcept :
            MVector({ x, y, z })
        {
        }

        Vector3U(Util::FXMVECTOR vec);
        Vector3U(Util::XMUINT3 vec);

        operator Util::XMVECTOR() const;
        operator Util::XMUINT3() const;

    public:
        AME_VECTOR_IMPL_MATHVEC_DECLARATIONS(Vector3I);

    public:
        AME_VECTOR_IMPL_ACCESSOR(0, x);
        AME_VECTOR_IMPL_ACCESSOR(1, y);
        AME_VECTOR_IMPL_ACCESSOR(2, z);
    };

    //

    class Vector4 : public Impl::MVector<float, 4>
    {
    public:
        using MVector::MVector;
        using Constants = Impl::VectorConstants<Vector4>;

        constexpr Vector4(float x, float y, float z, float w) noexcept :
            MVector({ x, y, z, w })
        {
        }

        Vector4(Util::FXMVECTOR vec);
        Vector4(Util::XMFLOAT4 vec);

        operator Util::XMVECTOR() const;
        operator Util::XMFLOAT4() const;

    public:
        [[nodiscard]] Vector4 Cross(const Vector4& Other1, const Vector4& Other2) const noexcept;

    public:
        AME_VECTOR_IMPL_MATHVEC_DECLARATIONS(Vector4);

    public:
        AME_VECTOR_IMPL_ACCESSOR(0, x);
        AME_VECTOR_IMPL_ACCESSOR(1, y);
        AME_VECTOR_IMPL_ACCESSOR(2, z);
        AME_VECTOR_IMPL_ACCESSOR(3, w);
    };

    class Vector4I : public Impl::MVector<int, 4>
    {
    public:
        using MVector::MVector;
        using Constants = Impl::VectorConstants<Vector4I>;

        constexpr Vector4I(int x, int y, int z, int w) noexcept :
            MVector({ x, y, z, w })
        {
        }

        Vector4I(Util::FXMVECTOR vec);
        Vector4I(Util::XMINT4 vec);

        operator Util::XMVECTOR() const;
        operator Util::XMINT4() const;

    public:
        AME_VECTOR_IMPL_MATHVEC_DECLARATIONS(Vector4I);

    public:
        AME_VECTOR_IMPL_ACCESSOR(0, x);
        AME_VECTOR_IMPL_ACCESSOR(1, y);
        AME_VECTOR_IMPL_ACCESSOR(2, z);
        AME_VECTOR_IMPL_ACCESSOR(3, w);
    };

    class Vector4U : public Impl::MVector<uint32_t, 4>
    {
    public:
        using MVector::MVector;
        using Constants = Impl::VectorConstants<Vector4U>;

        constexpr Vector4U(uint32_t x, uint32_t y, uint32_t z, uint32_t w) noexcept :
            MVector({ x, y, z, w })
        {
        }

        Vector4U(Util::FXMVECTOR vec);
        Vector4U(Util::XMUINT4 vec);

        operator Util::XMVECTOR() const;
        operator Util::XMUINT4() const;

    public:
        AME_VECTOR_IMPL_MATHVEC_DECLARATIONS(Vector4I);

    public:
        AME_VECTOR_IMPL_ACCESSOR(0, x);
        AME_VECTOR_IMPL_ACCESSOR(1, y);
        AME_VECTOR_IMPL_ACCESSOR(2, z);
        AME_VECTOR_IMPL_ACCESSOR(3, w);
    };

    //

    class Quaternion : public Vector4
    {
    public:
        using Vector4::Vector4;
        static const Quaternion Identity;

    public:
        [[nodiscard]] static Quaternion RotationRollPitchYaw(const Vector3& vec);

        [[nodiscard]] static Quaternion RotationAxis(const Vector3& axis, float angle);

        [[nodiscard]] static Quaternion RotationNormal(const Vector3& axis, float angle);

        [[nodiscard]] static Quaternion RotationMatrix(const Matrix4x4& matrix);

        [[nodiscard]] static Quaternion RotationMatrix(const Matrix3x3& matrix);

        [[nodiscard]] static Quaternion Slerp(const Quaternion& begin, const Quaternion& end, float t);

    public:
        AME_VECTOR_IMPL_ACCESSOR(0, x);
        AME_VECTOR_IMPL_ACCESSOR(1, y);
        AME_VECTOR_IMPL_ACCESSOR(2, z);
        AME_VECTOR_IMPL_ACCESSOR(3, w);

    public:
        void Normalize();

        Vector3 GetAxisAngle(float& angle) const;

        void Conjugate();

        [[nodiscard]] Vector3 Rotate(const Vector3& vec) const;

    public:
        AME_VECTOR_IMPL_MATH_OP(Quaternion, +);
        AME_VECTOR_IMPL_MATH_OP(Quaternion, -);
        AME_VECTOR_IMPL_MATH_OP(Quaternion, /);
        AME_VECTOR_IMPL_MATH_OP(Quaternion, *);
    };

    //

    inline constexpr Quaternion Quaternion::Identity = Quaternion(0.f, 0.f, 0.f, 1.f);
} // namespace Ame::Math

namespace Ame::Concepts
{
    template<typename Ty>
    concept VectorType = std::is_same_v<Ty, Math::Vector2> ||
                         std::is_same_v<Ty, Math::Vector2U> ||
                         std::is_same_v<Ty, Math::Vector2I> ||
                         std::is_same_v<Ty, Math::Vector3> ||
                         std::is_same_v<Ty, Math::Vector3I> ||
                         std::is_same_v<Ty, Math::Vector3U> ||
                         std::is_same_v<Ty, Math::Vector4> ||
                         std::is_same_v<Ty, Math::Vector4I> ||
                         std::is_same_v<Ty, Math::Vector4U>;

    template<typename Ty>
    concept QuaternionType = std::is_same_v<Ty, Math::Quaternion>;
} // namespace Ame::Concepts
