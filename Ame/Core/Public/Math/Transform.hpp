#pragma once

#include <Math/Matrix.hpp>
#include <Math/Vector.hpp>

#include <glm/gtx/matrix_interpolation.hpp>
#include <glm/gtx/transform.hpp>

namespace boost::serialization
{
    class access;
} // namespace boost::serialization

namespace Ame::Math
{
    class TransformMatrix
    {
    public:
        TransformMatrix(
            const Matrix3x3& basis    = Mat::c_Identity<Matrix3x3>,
            const Vector3&   position = Vec::c_Zero<Vector3>) :
            m_Basis(basis),
            m_Position(position)
        {
        }

        TransformMatrix(
            const Matrix4x4& transform) :
            m_Basis(transform),
            m_Position(transform[3])
        {
        }

    public:
        /// <summary>
        /// Get basis of transform
        /// </summary>
        [[nodiscard]] Matrix3x3 GetBasisNormalized() const noexcept
        {
            auto basis = m_Basis;
            basis[0]   = glm::normalize(basis[0]);
            basis[1]   = glm::normalize(basis[1]);
            basis[2]   = glm::normalize(basis[2]);
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
        void SetBasis(
            const Matrix3x3& basis) noexcept
        {
            m_Basis = basis;
        }

    public:
        /// <summary>
        /// Get rotation of transform
        /// </summary>
        [[nodiscard]] Quaternion GetRotation() const noexcept
        {
            return glm::quat_cast(GetBasisNormalized());
        }

        /// <summary>
        /// Set rotation of transform
        /// </summary>
        void SetRotation(
            const Quaternion& rotation) noexcept
        {
            auto scale = GetScale();
            m_Basis    = glm::mat3_cast(rotation);
            SetScale(scale);
        }

        /// <summary>
        /// Accumulate rotation of transform
        /// </summary>
        void AppendRotation(
            const Quaternion& rotation) noexcept
        {
            SetRotation(GetRotation() * rotation);
        }

    public:
        /// <summary>
        /// Get rotation of transform
        /// </summary>
        [[nodiscard]] Vector3 GetRotationEuler() const noexcept
        {
            return glm::eulerAngles(glm::quat_cast(GetBasisNormalized()));
        }

        /// <summary>
        /// Set rotation of transform
        /// </summary>
        void SetRotationEuler(
            const Vector3& rotation) noexcept
        {
            SetRotation(Quaternion(rotation));
        }

        /// <summary>
        /// Accumulate rotation of transform
        /// </summary>
        void AppendRotationEuler(
            const Vector3& rotation) noexcept
        {
            SetRotation(GetRotation() * Quaternion(rotation));
        }

    public:
        /// <summary>
        /// Get axis angle of transform
        /// </summary>
        [[nodiscard]] float GetAxisAngle(
            Vector3& axis) const noexcept
        {
            float angle;
            glm::axisAngle(Matrix4x4(GetBasisNormalized()), axis, angle);
            return angle;
        }

        /// <summary>
        /// Set axis angle of transform
        /// </summary>
        void SetAxisAngle(
            const Vector3& axis,
            float          angle) noexcept
        {
            SetRotation(glm::angleAxis(angle, axis));
        }

    public:
        /// <summary>
        /// Get right direction of transform
        /// </summary>
        [[nodiscard]] Vector3 GetRightDir() const noexcept
        {
            return glm::normalize(GetBasis()[0]);
        }

        /// <summary>
        /// Get right direction of transform
        /// </summary>
        [[nodiscard]] Vector3 GetUpDir() const noexcept
        {
            return glm::normalize(GetBasis()[1]);
        }

        /// <summary>
        /// Get right direction of transform
        /// </summary>
        [[nodiscard]] Vector3 GetLookDir() const noexcept
        {
            return glm::normalize(GetBasis()[2]);
        }

    public:
        /// <summary>
        /// Apply pitch rotation to transform
        /// </summary>
        void AppendPitch(
            float delta)
        {
            auto& right = GetBasis()[0];
            auto& up    = GetBasis()[1];
            auto& look  = GetBasis()[2];

            Matrix4x4 rotation = glm::rotate(delta, right);

            look = glm::normalize(rotation * Vector4(look, 0.f));
            up   = glm::normalize(rotation * Vector4(up, 0.f));
        }

        /// <summary>
        /// Apply yaw rotation to transform
        /// </summary>
        void AppendYaw(
            float delta)
        {
            auto& right = GetBasis()[0];
            auto& up    = GetBasis()[1];
            auto& look  = GetBasis()[2];

            Matrix4x4 rotation = glm::rotate(delta, Vec::c_Up<Vector3>);

            right = glm::normalize(rotation * Vector4(right, 0.f));
            look  = glm::normalize(rotation * Vector4(look, 0.f));
            up    = glm::normalize(rotation * Vector4(up, 0.f));
        }

        /// <summary>
        /// Apply roll rotation to transform
        /// </summary>
        void AppendRoll(
            float delta)
        {
            auto& right = GetBasis()[0];
            auto& up    = GetBasis()[1];
            auto& look  = GetBasis()[2];

            Matrix4x4 rotation = glm::rotate(delta, look);

            right = glm::normalize(rotation * Vector4(right, 0.f));
            up    = glm::normalize(rotation * Vector4(up, 0.f));
        }

    public:
        /// <summary>
        /// Get scale of transform
        /// </summary>
        [[nodiscard]] Vector3 GetScale() const noexcept
        {
            return {
                glm::length(m_Basis[0]),
                glm::length(m_Basis[1]),
                glm::length(m_Basis[2])
            };
        }

        /// <summary>
        /// Set scale of transform
        /// </summary>
        void SetScale(
            const Vector3& scale) noexcept
        {
            m_Basis[0] = glm::normalize(m_Basis[0]) * scale.x;
            m_Basis[1] = glm::normalize(m_Basis[1]) * scale.y;
            m_Basis[2] = glm::normalize(m_Basis[2]) * scale.z;
        }

        /// <summary>
        /// Accumulate scale of transform
        /// </summary>
        void AppendScale(
            const Vector3& scale) noexcept
        {
            SetScale(GetScale() * scale);
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
        void SetPosition(
            const Vector3& position) noexcept
        {
            m_Position = position;
        }

    public:
        /// <summary>
        /// Convert transform to matrix
        /// </summary>
        [[nodiscard]] Matrix4x4 ToMat4x4() const noexcept
        {
            return {
                Vector4(m_Basis[0], 0.f),
                Vector4(m_Basis[1], 0.f),
                Vector4(m_Basis[2], 0.f),
                Vector4(m_Position, 1.f)
            };
        }

        /// <summary>
        /// Convert transform to matrix
        /// </summary>
        [[nodiscard]] Matrix4x4 ToMat4x4Transposed() const noexcept
        {
            return Matrix4x4(
                m_Basis[0][0], m_Basis[1][0], m_Basis[2][0], m_Position.x,
                m_Basis[0][1], m_Basis[1][1], m_Basis[2][1], m_Position.y,
                m_Basis[0][2], m_Basis[1][2], m_Basis[2][2], m_Position.z,
                0.f, 0.f, 0.f, 1.f);
        }

    public:
        [[nodiscard]] TransformMatrix operator*(
            const TransformMatrix& other) const noexcept
        {
            return TransformMatrix(GetBasis() * other.GetBasis(), GetPosition() + other.GetPosition());
        }

        TransformMatrix& operator*=(
            const TransformMatrix& other) noexcept
        {
            m_Basis *= other.m_Basis;
            m_Position += other.m_Position;
            return *this;
        }

    private:
        friend class boost::serialization::access;

        template<typename ArchiveTy>
        void Serialize(
            ArchiveTy& archive,
            uint32_t)
        {
            archive & m_Basis & m_Position;
        }

    private:
        Matrix3x3 m_Basis;
        Vector3   m_Position;
    };
} // namespace Ame::Math