#pragma once

#include <tuple>
#include <Math/Vector.hpp>
#include <Math/Plane.hpp>

namespace Ame::Math
{
    class [[nodiscard]] AME_ENGINE_API Matrix4x4
    {
    public:
        struct Constants;

    public:
        constexpr Matrix4x4() = default;
        constexpr Matrix4x4(const Vector4& row1, const Vector4& row2, const Vector4& row3, const Vector4& row4) :
            m_Data{ row1, row2, row3, row4 }
        {
        }
        constexpr Matrix4x4(const Matrix3x3& matrix);

    public:
        Matrix4x4(Util::XMMATRIX matrix);
        Matrix4x4(Util::XMFLOAT4X4 matrix);

        operator Util::XMMATRIX() const;
        operator Util::XMFLOAT4X4() const;

    public:
        [[nodiscard]] constexpr auto data() const noexcept
        {
            return m_Data.data();
        }

        [[nodiscard]] auto data() noexcept
        {
            return m_Data.data();
        }

    public:
        [[nodiscard]] static Matrix4x4 Translation(const Vector3& vec);

        [[nodiscard]] static Matrix4x4 Scale(const Vector3& vec);

        [[nodiscard]] static Matrix4x4 RotationX(float angle);

        [[nodiscard]] static Matrix4x4 RotationY(float angle);

        [[nodiscard]] static Matrix4x4 RotationZ(float angle);

        [[nodiscard]] static Matrix4x4 RotationRollPitchYaw(const Vector3& vec);

        [[nodiscard]] static Matrix4x4 RotationQuaternion(const Quaternion& quat);

        [[nodiscard]] static Matrix4x4 RotationNormal(const Vector3& axis, float angle);

        [[nodiscard]] static Matrix4x4 RotationAxis(const Vector3& axis, float angle);

        [[nodiscard]] static Matrix4x4 LookAt(const Vector3& eyePosition, const Vector3& focusPosition,
                                              const Vector3& upDirection);

        [[nodiscard]] static Matrix4x4 LookTo(const Vector3& eyePosition, const Vector3& eyeDirection,
                                              const Vector3& upDirection);

        [[nodiscard]] static Matrix4x4 Perspective(float viewWidth, float viewHeight, float nearZ, float farZ);

        [[nodiscard]] static Matrix4x4 PerspectiveFov(float fovAngleY, float aspectRatio, float nearZ, float farZ);

        [[nodiscard]] static Matrix4x4 Orthographic(float viewWidth, float viewHeight, float nearZ, float farZ);

        [[nodiscard]] static Matrix4x4 SRT(const Matrix4x4& scale, const Matrix4x4& rotation,
                                           const Matrix4x4& translation);

    public:
        [[nodiscard]] Vector2 DoTransform(const Vector2& vec) const;
        [[nodiscard]] Vector2 DoTransformCoord(const Vector2& vec) const;
        [[nodiscard]] Vector2 DoTransformNormal(const Vector2& vec) const;

        [[nodiscard]] Vector3 DoTransform(const Vector3& vec) const;
        [[nodiscard]] Vector3 DoTransformCoord(const Vector3& vec) const;
        [[nodiscard]] Vector3 DoTransformNormal(const Vector3& vec) const;

        [[nodiscard]] Vector4 DoTransform(const Vector4& vec) const;
        [[nodiscard]] Plane   DoTransform(const Plane& plane) const;

        [[nodiscard]] Matrix4x4 GetInverse() const;

        [[nodiscard]] Matrix4x4 GetTranspose() const;

        [[nodiscard]] Vector4 GetDeterminant() const;

        [[nodiscard]] constexpr float& operator()(size_t row, size_t column) noexcept
        {
            return m_Data[row][column];
        }

        [[nodiscard]] constexpr Vector4& operator()(size_t row) noexcept
        {
            return m_Data[row];
        }

        [[nodiscard]] constexpr float operator()(size_t row, size_t column) const noexcept
        {
            return m_Data[row][column];
        }

        [[nodiscard]] constexpr const Vector4& operator()(size_t row) const noexcept
        {
            return m_Data[row];
        }

    public:
        [[nodiscard]] std::tuple<Vector4, Quaternion, Vector3> GetPositionRotationScale() const;

        [[nodiscard]] Vector3 GetScale() const;
        void                  SetScale(const Vector3& scale);

        [[nodiscard]] Quaternion GetRotationQuat() const;

        [[nodiscard]] Vector3 GetPosition() const;
        void                  SetPosition(const Vector3& position)
        {
            SetPosition(Vector4(position.x(), position.y(), position.z(), 1.f));
        }
        void SetPosition(const Vector4& position);

        Matrix4x4  operator*(const Matrix4x4& other) const;
        Matrix4x4& operator*=(const Matrix4x4& other);

    private:
        [[nodiscard]] static constexpr Matrix4x4 _Identity()
        {
            Matrix4x4 Mat;
            Mat(0, 0) = Mat(1, 1) = Mat(2, 2) = Mat(3, 3) = 1.f;
            return Mat;
        }

    private:
        std::array<Vector4, 4> m_Data;
    };

    class [[nodiscard]] AME_ENGINE_API Matrix3x3
    {
    public:
        struct Constants;

    public:
        constexpr Matrix3x3() = default;
        constexpr Matrix3x3(const Vector3& row1, const Vector3& row2, const Vector3& row3) : m_Data{ row1, row2, row3 }
        {
        }
        constexpr Matrix3x3(const Matrix4x4& matrix) : m_Data({ matrix(0), matrix(1), matrix(2) })
        {
        }

    public:
        Matrix3x3(Util::XMMATRIX matrix);
        Matrix3x3(Util::XMFLOAT3X3 matrix);

        operator Util::XMMATRIX() const;
        operator Util::XMFLOAT3X3() const;

    public:
        [[nodiscard]] constexpr auto data() const noexcept
        {
            return m_Data.data();
        }

        [[nodiscard]] auto data() noexcept
        {
            return m_Data.data();
        }

    public:
        [[nodiscard]] static Matrix3x3 Scale(const Vector3& vec);

        [[nodiscard]] static Matrix3x3 RotationX(float angle);

        [[nodiscard]] static Matrix3x3 RotationY(float angle);

        [[nodiscard]] static Matrix3x3 RotationZ(float angle);

        [[nodiscard]] static Matrix3x3 RotationRollPitchYaw(const Vector3& vec);

        [[nodiscard]] static Matrix3x3 RotationQuaternion(const Quaternion& quat);

        [[nodiscard]] static Matrix3x3 RotationNormal(const Vector3& axis, float angle);

        [[nodiscard]] static Matrix3x3 RotationAxis(const Vector3& axis, float angle);

        [[nodiscard]] static Matrix3x3 SR(const Matrix3x3& scale, const Matrix3x3& rotation);

    public:
        [[nodiscard]] Matrix3x3 GetInverse() const;

        [[nodiscard]] Matrix3x3 GetTranspose() const;

        [[nodiscard]] Vector4 GetDeterminant() const;

        [[nodiscard]] constexpr float& operator()(size_t row, size_t column) noexcept
        {
            return m_Data[row][column];
        }

        [[nodiscard]] constexpr Vector3& operator()(size_t row) noexcept
        {
            return m_Data[row];
        }

        [[nodiscard]] constexpr float operator()(size_t row, size_t column) const noexcept
        {
            return m_Data[row][column];
        }

        [[nodiscard]] constexpr const Vector3& operator()(size_t row) const noexcept
        {
            return m_Data[row];
        }

    public:
        [[nodiscard]] std::pair<Quaternion, Vector3> GetRotationScale() const;
        void                                         SetRotationScale(const Quaternion& rotation, const Vector3& scale);

        [[nodiscard]] Vector3 GetScale() const;
        void                  SetScale(const Vector3& scale);

        [[nodiscard]] Quaternion GetRotationQuat() const;
        void                     SetRotationQuat(const Quaternion& quat);

        Matrix3x3  operator*(const Matrix3x3& other) const;
        Matrix3x3& operator*=(const Matrix3x3& other);

    private:
        std::array<Vector3, 3> m_Data;
    };

    //

    inline constexpr Matrix4x4::Matrix4x4(const Matrix3x3& matrix) : m_Data({ matrix(0), matrix(1), matrix(2), {} })
    {
    }

    //

    struct Matrix4x4::Constants
    {
        static constexpr Matrix4x4 Zero{};
        static constexpr Matrix4x4 Identity{ Vector4{ 1.f, 0.f, 0.f, 0.f }, Vector4{ 0.f, 1.f, 0.f, 0.f },
                                             Vector4{ 0.f, 0.f, 1.f, 0.f }, Vector4{ 0.f, 0.f, 0.f, 1.f } };
    };

    //

    struct Matrix3x3::Constants
    {
        static constexpr Matrix3x3 Zero{};
        static constexpr Matrix3x3 Identity{ Vector3{ 1.f, 0.f, 0.f }, Vector3{ 0.f, 1.f, 0.f },
                                             Vector3{ 0.f, 0.f, 1.f } };
    };
} // namespace Ame::Math

namespace Ame::Concepts
{
    template<typename Ty>
    concept MatrixType = std::is_same_v<Ty, Math::Matrix3x3> || std::is_same_v<Ty, Math::Matrix4x4>;
} // namespace Ame::Concepts
