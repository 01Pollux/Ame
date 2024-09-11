#include <Math/Matrix.hpp>

namespace Ame::Math
{
    using namespace Util;

    //

#define AME_IMPL_MATHVEC_DEFINITIONS(Class, Dim, Type, XMType, XMVecReplicateFunc, XMVecGetXFunc)                      \
    bool Class::IsNan() const noexcept                                                                                 \
    {                                                                                                                  \
        auto res = XMVectorSelect(g_XMOne, g_XMZero, XMVectorIsNaN(*this));                                            \
        return XMVecGetXFunc(res) == 1;                                                                                \
    }                                                                                                                  \
                                                                                                                       \
    void Class::Negate() noexcept                                                                                      \
    {                                                                                                                  \
        *this = XMVectorNegate(*this);                                                                                 \
    }                                                                                                                  \
                                                                                                                       \
    Class::value_type Class::Dot(const Class& other) const noexcept                                                    \
    {                                                                                                                  \
        auto res = XMVector##Dim##Dot(*this, other);                                                                   \
        return XMVecGetXFunc(res);                                                                                     \
    }                                                                                                                  \
                                                                                                                       \
    Class::value_type Class::LengthSqr() const noexcept                                                                \
    {                                                                                                                  \
        auto res = XMVector##Dim##LengthSq(*this);                                                                     \
        return XMVecGetXFunc(res);                                                                                     \
    }                                                                                                                  \
                                                                                                                       \
    Class::value_type Class::Length() const noexcept                                                                   \
    {                                                                                                                  \
        auto res = XMVector##Dim##Length(*this);                                                                       \
        return XMVecGetXFunc(res);                                                                                     \
    }                                                                                                                  \
                                                                                                                       \
    bool Class::IsGreaterThan(const Class& other) const noexcept                                                       \
    {                                                                                                                  \
        return XMVector##Dim##Greater(*this, other);                                                                   \
    }                                                                                                                  \
                                                                                                                       \
    bool Class::IsLessThan(const Class& other) const noexcept                                                          \
    {                                                                                                                  \
        return XMVector##Dim##Less(*this, other);                                                                      \
    }                                                                                                                  \
                                                                                                                       \
    void Class::Normalize() noexcept                                                                                   \
    {                                                                                                                  \
        *this = XMVector##Dim##Normalize(*this);                                                                       \
    }                                                                                                                  \
                                                                                                                       \
    bool Class::NearEqual(const Class& other, value_type tolerance) const noexcept                                     \
    {                                                                                                                  \
        return XMVector##Dim##NearEqual(*this, other, XMVecReplicateFunc(tolerance));                                  \
    }                                                                                                                  \
                                                                                                                       \
    bool Class::Within(const Class& min, const Class& max) const noexcept                                              \
    {                                                                                                                  \
        XMVECTOR vec = *this;                                                                                          \
        return XMVector##Dim##GreaterOrEqual(vec, min) && XMVector##Dim##LessOrEqual(vec, max);                        \
    }                                                                                                                  \
                                                                                                                       \
    void Class::MultAdd(const Class& a, const Class& b) noexcept                                                       \
    {                                                                                                                  \
        *this = XMVectorMultiplyAdd(*this, a, b);                                                                      \
    }                                                                                                                  \
                                                                                                                       \
    Class& Class::operator=(value_type val) noexcept                                                                   \
    {                                                                                                                  \
        *this = XMVecReplicateFunc(val);                                                                               \
        return *this;                                                                                                  \
    }                                                                                                                  \
                                                                                                                       \
    Class Class::operator-() const noexcept                                                                            \
    {                                                                                                                  \
        return XMVectorNegate(*this);                                                                                  \
    }                                                                                                                  \
                                                                                                                       \
    Class Class::min(const Class& other) const noexcept                                                                \
    {                                                                                                                  \
        return XMVectorMin(*this, other);                                                                              \
    }                                                                                                                  \
                                                                                                                       \
    Class Class::max(const Class& other) const noexcept                                                                \
    {                                                                                                                  \
        return XMVectorMax(*this, other);                                                                              \
    }                                                                                                                  \
                                                                                                                       \
    Class& Class::operator+=(const Class& other) noexcept                                                              \
    {                                                                                                                  \
        *this = XMVectorAdd(*this, other);                                                                             \
        return *this;                                                                                                  \
    }                                                                                                                  \
                                                                                                                       \
    Class& Class::operator+=(value_type val) noexcept                                                                  \
    {                                                                                                                  \
        *this = XMVectorAdd(*this, XMVecReplicateFunc(val));                                                           \
        return *this;                                                                                                  \
    }                                                                                                                  \
                                                                                                                       \
    Class& Class::operator-=(const Class& other) noexcept                                                              \
    {                                                                                                                  \
        *this = XMVectorSubtract(*this, other);                                                                        \
        return *this;                                                                                                  \
    }                                                                                                                  \
                                                                                                                       \
    Class& Class::operator-=(value_type val) noexcept                                                                  \
    {                                                                                                                  \
        *this = XMVectorSubtract(*this, XMVecReplicateFunc(val));                                                      \
        return *this;                                                                                                  \
    }                                                                                                                  \
                                                                                                                       \
    Class& Class::operator*=(const Class& other) noexcept                                                              \
    {                                                                                                                  \
        *this = XMVectorMultiply(*this, other);                                                                        \
        return *this;                                                                                                  \
    }                                                                                                                  \
                                                                                                                       \
    Class& Class::operator*=(value_type val) noexcept                                                                  \
    {                                                                                                                  \
        *this = XMVectorMultiply(*this, XMVecReplicateFunc(val));                                                      \
        return *this;                                                                                                  \
    }                                                                                                                  \
                                                                                                                       \
    Class& Class::operator/=(const Class& other) noexcept                                                              \
    {                                                                                                                  \
        *this = XMVectorDivide(*this, other);                                                                          \
        return *this;                                                                                                  \
    }                                                                                                                  \
                                                                                                                       \
    Class& Class::operator/=(value_type val) noexcept                                                                  \
    {                                                                                                                  \
        *this = XMVectorDivide(*this, XMVecReplicateFunc(val));                                                        \
        return *this;                                                                                                  \
    }

    //

    AME_IMPL_MATHVEC_DEFINITIONS(Vector2, 2, Float, XMFLOAT2, XMVectorReplicate, XMVectorGetX);
    AME_IMPL_MATHVEC_DEFINITIONS(Vector3, 3, Float, XMFLOAT3, XMVectorReplicate, XMVectorGetX);
    AME_IMPL_MATHVEC_DEFINITIONS(Vector4, 4, Float, XMFLOAT4, XMVectorReplicate, XMVectorGetX);

    AME_IMPL_MATHVEC_DEFINITIONS(Vector2I, 2, Int, uint32_t, XMVectorReplicateInt, XMVectorGetIntX);
    AME_IMPL_MATHVEC_DEFINITIONS(Vector3I, 3, Int, uint32_t, XMVectorReplicateInt, XMVectorGetIntX);
    AME_IMPL_MATHVEC_DEFINITIONS(Vector4I, 4, Int, uint32_t, XMVectorReplicateInt, XMVectorGetIntX);

    //

    Vector2::Vector2(Util::FXMVECTOR vec)
    {
        XMStoreFloat2(std::bit_cast<XMFLOAT2*>(data()), vec);
    }

    Vector2::Vector2(Util::XMFLOAT2 vec) : Vector2(vec.x, vec.y)
    {
    }

    Vector2::operator Util::XMVECTOR() const
    {
        return XMLoadFloat2(std::bit_cast<const XMFLOAT2*>(data()));
    }

    Vector2::operator Util::XMFLOAT2() const
    {
        return { x(), y() };
    }

    //

    float Vector2::Cross(const Vector2& other) const noexcept
    {
        auto res = XMVector2Cross(*this, other);
        return XMVectorGetX(res);
    }

    //

    Vector2I::Vector2I(Util::FXMVECTOR vec)
    {
        XMStoreInt2(std::bit_cast<uint32_t*>(data()), vec);
    }

    Vector2I::Vector2I(Util::XMINT2 vec) : Vector2I(vec.x, vec.y)
    {
    }

    Vector2I::operator Util::XMVECTOR() const
    {
        return XMLoadInt2(std::bit_cast<const uint32_t*>(data()));
    }

    Vector2I::operator Util::XMINT2() const
    {
        return { x(), y() };
    }

    //

    Vector2U::Vector2U(Util::FXMVECTOR vec)
    {
        XMStoreInt2(data(), vec);
    }

    Vector2U::Vector2U(Util::XMUINT2 vec) : Vector2U(vec.x, vec.y)
    {
    }

    Vector2U::operator Util::XMVECTOR() const
    {
        return XMLoadInt2(data());
    }

    Vector2U::operator Util::XMUINT2() const
    {
        return { x(), y() };
    }

    //

    Vector3::Vector3(Util::FXMVECTOR vec)
    {
        XMStoreFloat3(std::bit_cast<XMFLOAT3*>(data()), vec);
    }

    Vector3::Vector3(Util::XMFLOAT3 vec) : Vector3(vec.x, vec.y, vec.z)
    {
    }

    Vector3::operator Util::XMVECTOR() const
    {
        return XMLoadFloat3(std::bit_cast<const XMFLOAT3*>(data()));
    }

    Vector3::operator Util::XMFLOAT3() const
    {
        return { x(), y(), z() };
    }

    //

    Vector3 Vector3::Cross(const Vector3& other) const noexcept
    {
        return XMVector3Cross(*this, other);
    }

    //

    Vector3I::Vector3I(Util::FXMVECTOR vec)
    {
        XMStoreInt3(std::bit_cast<uint32_t*>(data()), vec);
    }

    Vector3I::Vector3I(Util::XMINT3 vec) : Vector3I(vec.x, vec.y, vec.z)
    {
    }

    Vector3I::operator Util::XMVECTOR() const
    {
        return XMLoadInt3(std::bit_cast<const uint32_t*>(data()));
    }

    Vector3I::operator Util::XMINT3() const
    {
        return { x(), y(), z() };
    }

    //

    Vector3U::Vector3U(Util::FXMVECTOR vec)
    {
        XMStoreInt3(data(), vec);
    }

    Vector3U::Vector3U(Util::XMUINT3 vec) : Vector3U(vec.x, vec.y, vec.z)
    {
    }

    Vector3U::operator Util::XMVECTOR() const
    {
        return XMLoadInt3(data());
    }

    Vector3U::operator Util::XMUINT3() const
    {
        return { x(), y(), z() };
    }

    //

    Vector4::Vector4(Util::FXMVECTOR vec)
    {
        XMStoreFloat4(std::bit_cast<XMFLOAT4*>(data()), vec);
    }

    Vector4::Vector4(Util::XMFLOAT4 vec) : Vector4(vec.x, vec.y, vec.z, vec.w)
    {
    }

    Vector4::operator Util::XMVECTOR() const
    {
        return XMLoadFloat4(std::bit_cast<const XMFLOAT4*>(data()));
    }

    Vector4::operator Util::XMFLOAT4() const
    {
        return { x(), y(), z(), w() };
    }

    //

    Vector4 Vector4::Cross(const Vector4& other1, const Vector4& other2) const noexcept
    {
        return XMVector4Cross(*this, other1, other2);
    }

    //

    Vector4I::Vector4I(Util::FXMVECTOR vec)
    {
        XMStoreInt3(std::bit_cast<uint32_t*>(data()), vec);
    }

    Vector4I::Vector4I(Util::XMINT4 vec) : Vector4I(vec.x, vec.y, vec.z, vec.w)
    {
    }

    Vector4I::operator Util::XMVECTOR() const
    {
        return XMLoadInt4(std::bit_cast<const uint32_t*>(data()));
    }

    Vector4I::operator Util::XMINT4() const
    {
        return { x(), y(), z(), w() };
    }

    //

    Vector4U::Vector4U(Util::FXMVECTOR vec)
    {
        XMStoreInt4(data(), vec);
    }

    Vector4U::Vector4U(Util::XMUINT4 vec) : Vector4U(vec.x, vec.y, vec.z, vec.w)
    {
    }

    Vector4U::operator Util::XMVECTOR() const
    {
        return XMLoadInt4(data());
    }

    Vector4U::operator Util::XMUINT4() const
    {
        return { x(), y(), z(), w() };
    }

    //

    Quaternion Quaternion::RotationRollPitchYaw(const Vector3& vec)
    {
        return XMQuaternionRotationRollPitchYaw(vec.x(), vec.y(), vec.z());
    }

    Quaternion Quaternion::RotationAxis(const Vector3& axis, float angle)
    {
        return XMQuaternionRotationAxis(axis, angle);
    }

    Quaternion Quaternion::RotationNormal(const Vector3& axis, float angle)
    {
        return XMQuaternionRotationNormal(axis, angle);
    }

    Quaternion Quaternion::RotationMatrix(const Matrix4x4& matrix)
    {
        return XMQuaternionRotationMatrix(matrix);
    }

    Quaternion Quaternion::RotationMatrix(const Matrix3x3& matrix)
    {
        return XMQuaternionRotationMatrix(matrix);
    }

    Quaternion Quaternion::Slerp(const Quaternion& begin, const Quaternion& end, float t)
    {
        return XMQuaternionSlerp(begin, end, t);
    }

    void Quaternion::Normalize()
    {
        *this = XMQuaternionNormalize(*this);
    }

    Vector3 Quaternion::GetAxisAngle(float& angle) const
    {
        Util::XMVECTOR axis;
        XMQuaternionToAxisAngle(&axis, &angle, *this);
        return axis;
    }

    void Quaternion::Conjugate()
    {
        *this = XMQuaternionConjugate(*this);
    }

    Vector3 Quaternion::Rotate(const Vector3& vec) const
    {
        return XMVector3Rotate(vec, *this);
    }

    Quaternion& Quaternion::operator+=(const Quaternion& other) noexcept
    {
        *this = XMVectorAdd(*this, other);
        return *this;
    }

    Quaternion& Quaternion::operator+=(value_type val) noexcept
    {
        *this = XMVectorAdd(*this, XMVectorReplicate(val));
        return *this;
    }

    Quaternion& Quaternion::operator-=(const Quaternion& other) noexcept
    {
        *this = XMVectorSubtract(*this, other);
        return *this;
    }

    Quaternion& Quaternion::operator-=(value_type val) noexcept
    {
        *this = XMVectorSubtract(*this, XMVectorReplicate(val));
        return *this;
    }

    Quaternion& Quaternion::operator*=(const Quaternion& other) noexcept
    {
        *this = XMQuaternionMultiply(*this, other);
        return *this;
    }

    Quaternion& Quaternion::operator*=(value_type val) noexcept
    {
        *this = XMQuaternionMultiply(*this, XMVectorReplicate(val));
        return *this;
    }

    Quaternion& Quaternion::operator/=(const Quaternion& other) noexcept
    {
        *this = XMVectorDivide(*this, other);
        return *this;
    }

    Quaternion& Quaternion::operator/=(value_type val) noexcept
    {
        *this = XMVectorDivide(*this, XMVectorReplicate(val));
        return *this;
    }
} // namespace Ame::Math
