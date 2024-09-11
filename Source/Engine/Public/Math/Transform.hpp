#pragma once

#include <Math/Matrix.hpp>
#include <Math/Vector.hpp>

namespace Ame::Math
{
    class TransformMatrix
    {
    public:
        static constexpr uint32_t RightDirIndex = 0;
        static constexpr uint32_t UpDirIndex    = 1;
        static constexpr uint32_t LookDirIndex  = 2;

    public:
        TransformMatrix(const Matrix3x3& basis    = Matrix3x3::Constants::Identity,
                        const Vector3&   position = Vector3::Constants::Zero) : m_Basis(basis), m_Position(position)
        {
        }

        TransformMatrix(const Matrix4x4& transform) : m_Basis(transform), m_Position(transform(3))
        {
        }

    public:
        /// <summary>
        /// Get basis of transform
        /// </summary>
        [[nodiscard]] Matrix3x3 GetBasisNormalized() const noexcept
        {
            auto basis = m_Basis;

            basis(0).Normalize();
            basis(1).Normalize();
            basis(2).Normalize();

            return basis;
        }

        /// <summary>
        /// Get basis of transform
        /// </summary>
        [[nodiscard]] const Matrix3x3& GetBasis() const noexcept
        {
            return m_Basis;
        }

        /// <summary>
        /// Get basis of transform
        /// </summary>
        [[nodiscard]] Matrix3x3& GetBasis() noexcept
        {
            return m_Basis;
        }

        /// <summary>
        /// Set basis of transform
        /// </summary>
        void SetBasis(const Matrix3x3& basis) noexcept
        {
            m_Basis = basis;
        }

    public:
        /// <summary>
        /// Get right direction of transform
        /// </summary>
        [[nodiscard]] Vector3 GetRightDir() const noexcept
        {
            auto right = GetBasis()(RightDirIndex);
            right.Normalize();
            return right;
        }

        /// <summary>
        /// Get right direction of transform
        /// </summary>
        [[nodiscard]] Vector3 GetUpDir() const noexcept
        {
            auto up = GetBasis()(UpDirIndex);
            up.Normalize();
            return up;
        }

        /// <summary>
        /// Get right direction of transform
        /// </summary>
        [[nodiscard]] Vector3 GetLookDir() const noexcept
        {
            auto look = GetBasis()(LookDirIndex);
            look.Normalize();
            return look;
        }

    public:
        /// <summary>
        /// Apply pitch rotation to transform
        /// </summary>
        void AppendPitch(float delta)
        {
            auto& right = GetBasis()(0);
            auto& up    = GetBasis()(1);
            auto& look  = GetBasis()(2);

            Matrix4x4 rotation = Util::XMMatrixRotationAxis(right, delta);

            look = rotation.DoTransformNormal(look);
            up   = rotation.DoTransformNormal(up);
        }

        /// <summary>
        /// Apply yaw rotation to transform
        /// </summary>
        void AppendYaw(float delta)
        {
            auto& right = GetBasis()(0);
            auto& up    = GetBasis()(1);
            auto& look  = GetBasis()(2);

            Matrix4x4 rotation = Util::XMMatrixRotationNormal(Vector3::Constants::Up, delta);

            right = rotation.DoTransformNormal(right);
            look  = rotation.DoTransformNormal(look);
            up    = rotation.DoTransformNormal(up);
        }

        /// <summary>
        /// Apply roll rotation to transform
        /// </summary>
        void AppendRoll(float delta)
        {
            auto& right = GetBasis()(0);
            auto& up    = GetBasis()(1);
            auto& look  = GetBasis()(2);

            Matrix4x4 rotation = Util::XMMatrixRotationAxis(look, delta);

            right = rotation.DoTransformNormal(right);
            up    = rotation.DoTransformNormal(up);
        }

    public:
        /// <summary>
        /// Get position of transform
        /// </summary>
        [[nodiscard]] const Vector3& GetPosition() const noexcept
        {
            return m_Position;
        }

        /// <summary>
        /// Get position of transform
        /// </summary>
        [[nodiscard]] Vector3& GetPosition() noexcept
        {
            return m_Position;
        }

        /// <summary>
        /// Set position of transform
        /// </summary>
        void SetPosition(const Vector3& position) noexcept
        {
            m_Position = position;
        }

    public:
        /// <summary>
        /// Convert transform to matrix
        /// </summary>
        [[nodiscard]] Matrix4x4 ToMat4x4() const noexcept
        {
            return { m_Basis(0), m_Basis(1), m_Basis(2), Vector4(m_Position.x(), m_Position.y(), m_Position.z(), 1.f) };
        }

        /// <summary>
        /// Convert transform to matrix
        /// </summary>
        [[nodiscard]] Matrix4x4 ToMat4x4Transposed() const noexcept
        {
            return { { m_Basis(0).x(), m_Basis(1).x(), m_Basis(2).x(), m_Position.x() },
                     { m_Basis(0).y(), m_Basis(1).y(), m_Basis(2).y(), m_Position.y() },
                     { m_Basis(0).z(), m_Basis(1).z(), m_Basis(2).z(), m_Position.z() },
                     { 0.f, 0.f, 0.f, 1.f } };
        }

    public:
        [[nodiscard]] TransformMatrix operator*(const TransformMatrix& other) const noexcept
        {
            return TransformMatrix(GetBasis() * other.GetBasis(), GetPosition() + other.GetPosition());
        }

        TransformMatrix& operator*=(const TransformMatrix& other) noexcept
        {
            m_Basis *= other.m_Basis;
            m_Position += other.m_Position;
            return *this;
        }

    private:
        Matrix3x3 m_Basis;
        Vector3   m_Position;
    };
} // namespace Ame::Math