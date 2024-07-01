#include <Math/Matrix.hpp>

namespace Ame::Math
{
    using namespace Util;

    //

    Matrix4x4::Matrix4x4(Util::XMMATRIX matrix)
    {
        XMStoreFloat4x4(std::bit_cast<XMFLOAT4X4*>(this), matrix);
    }

    Matrix4x4::Matrix4x4(Util::XMFLOAT4X4 matrix)
    {
        static_assert(sizeof(matrix) == sizeof(*this));
        std::memcpy(this, &matrix, sizeof(Matrix4x4));
    }

    Matrix4x4::operator Util::XMMATRIX() const
    {
        return XMLoadFloat4x4(std::bit_cast<XMFLOAT4X4*>(this));
    }

    Matrix4x4::operator Util::XMFLOAT4X4() const
    {
        XMFLOAT4X4 matrix;
        std::memcpy(&matrix, this, sizeof(Matrix4x4));
        static_assert(sizeof(matrix) == sizeof(*this));
        return matrix;
    }

    //

    Matrix4x4 Matrix4x4::Translation(const Vector3& vec)
    {
        return XMMatrixTranslationFromVector(vec);
    }

    Matrix4x4 Matrix4x4::Scale(const Vector3& vec)
    {
        return XMMatrixScalingFromVector(vec);
    }

    Matrix4x4 Matrix4x4::RotationX(float angle)
    {
        return XMMatrixRotationX(angle);
    }

    Matrix4x4 Matrix4x4::RotationY(float angle)
    {
        return XMMatrixRotationY(angle);
    }

    Matrix4x4 Matrix4x4::RotationZ(float angle)
    {
        return XMMatrixRotationZ(angle);
    }

    Matrix4x4 Matrix4x4::RotationRollPitchYaw(const Vector3& vec)
    {
        return XMMatrixRotationRollPitchYaw(vec.x(), vec.y(), vec.z());
    }

    Matrix4x4 Matrix4x4::RotationQuaternion(const Quaternion& vec)
    {
        return XMMatrixRotationQuaternion(vec);
    }

    Matrix4x4 Matrix4x4::RotationNormal(const Vector3& axis, float angle)
    {
        return XMMatrixRotationNormal(axis, angle);
    }

    Matrix4x4 Matrix4x4::RotationAxis(const Vector3& axis, float angle)
    {
        return XMMatrixRotationAxis(axis, angle);
    }

    Matrix4x4 Matrix4x4::LookAt(const Vector3& eyePosition, const Vector3& focusPosition, const Vector3& upDirection)
    {
        return XMMatrixLookAtLH(eyePosition, focusPosition, upDirection);
    }

    Matrix4x4 Matrix4x4::LookTo(const Vector3& eyePosition, const Vector3& eyeDirection, const Vector3& upDirection)
    {
        return XMMatrixLookToLH(eyePosition, eyeDirection, upDirection);
    }

    Matrix4x4 Matrix4x4::Perspective(float viewWidth, float viewHeight, float nearZ, float farZ)
    {
        return XMMatrixPerspectiveLH(viewWidth, viewHeight, nearZ, farZ);
    }

    Matrix4x4 Matrix4x4::PerspectiveFov(float fovAngleY, float AspectRatio, float nearZ, float farZ)
    {
        return XMMatrixPerspectiveFovLH(fovAngleY, AspectRatio, nearZ, farZ);
    }

    Matrix4x4 Matrix4x4::Orthographic(float viewWidth, float viewHeight, float nearZ, float farZ)
    {
        return XMMatrixOrthographicLH(viewWidth, viewHeight, nearZ, farZ);
    }

    Matrix4x4 Matrix4x4::SRT(const Matrix4x4& scale, const Matrix4x4& rotation, const Matrix4x4& translation)
    {
        return scale * rotation * translation;
    }

    Vector2 Matrix4x4::DoTransform(const Vector2& vec) const
    {
        return XMVector2Transform(vec, *this);
    }

    Vector2 Matrix4x4::DoTransformCoord(const Vector2& vec) const
    {
        return XMVector2TransformCoord(vec, *this);
    }

    Vector2 Matrix4x4::DoTransformNormal(const Vector2& vec) const
    {
        return XMVector2TransformNormal(vec, *this);
    }

    Vector3 Matrix4x4::DoTransform(const Vector3& vec) const
    {
        return XMVector3Transform(vec, *this);
    }

    Vector3 Matrix4x4::DoTransformCoord(const Vector3& vec) const
    {
        return XMVector3TransformCoord(vec, *this);
    }

    Vector3 Matrix4x4::DoTransformNormal(const Vector3& vec) const
    {
        return XMVector3TransformNormal(vec, *this);
    }

    Vector4 Matrix4x4::DoTransform(const Vector4& vec) const
    {
        return XMVector4Transform(vec, *this);
    }

    Plane Matrix4x4::DoTransform(const Plane& plane) const
    {
        return XMPlaneTransform(plane, *this);
    }

    Matrix4x4 Matrix4x4::GetInverse() const
    {
        return XMMatrixInverse(nullptr, *this);
    }

    Matrix4x4 Matrix4x4::GetTranspose() const
    {
        return XMMatrixTranspose(*this);
    }

    Vector4 Matrix4x4::GetDeterminant() const
    {
        return XMMatrixDeterminant(*this);
    }

    std::tuple<Vector4, Quaternion, Vector3> Matrix4x4::GetPositionRotationScale() const
    {
        XMVECTOR position, rotation, scale;
        XMMatrixDecompose(&scale, &rotation, &position, *this);

        return { position, rotation, scale };
    }

    Vector3 Matrix4x4::GetScale() const
    {
        XMVECTOR scale, unused;
        XMMatrixDecompose(&scale, &unused, &unused, *this);

        return scale;
    }

    void Matrix4x4::SetScale(const Vector3& scale)
    {
        XMVECTOR xmPosition, xmRotation, unused;

        XMMatrixDecompose(&unused, &xmRotation, &xmPosition, *this);

        *this = XMMatrixScalingFromVector(scale) *
                XMMatrixRotationQuaternion(xmRotation) *
                XMMatrixTranslationFromVector(xmPosition);
    }

    Quaternion Matrix4x4::GetRotationQuat() const
    {
        return XMQuaternionRotationMatrix(*this);
    }

    Vector3 Matrix4x4::GetPosition() const
    {
        return { m_Data[3][0], m_Data[3][1], m_Data[3][2] };
    }

    void Matrix4x4::SetPosition(const Vector4& position)
    {
        m_Data[3] = position;
    }

    Matrix4x4 Matrix4x4::operator*(const Matrix4x4& other) const
    {
        return XMMatrixMultiply(*this, other);
    }

    Matrix4x4& Matrix4x4::operator*=(const Matrix4x4& other)
    {
        *this = XMMatrixMultiply(*this, other);
        return *this;
    }

    //

    Matrix3x3::Matrix3x3(Util::XMMATRIX matrix)
    {
        XMStoreFloat3x3(std::bit_cast<XMFLOAT3X3*>(this), matrix);
    }

    Matrix3x3::Matrix3x3(Util::XMFLOAT3X3 matrix)
    {
        static_assert(sizeof(matrix) == sizeof(*this));
        std::memcpy(this, &matrix, sizeof(Matrix3x3));
    }

    Matrix3x3::operator Util::XMMATRIX() const
    {
        return XMLoadFloat3x3(std::bit_cast<XMFLOAT3X3*>(this));
    }

    Matrix3x3::operator Util::XMFLOAT3X3() const
    {
        XMFLOAT3X3 matrix;
        std::memcpy(&matrix, this, sizeof(Matrix3x3));
        static_assert(sizeof(matrix) == sizeof(*this));
        return matrix;
    }

    //

    Matrix3x3 Matrix3x3::Scale(const Vector3& vec)
    {
        return XMMatrixScalingFromVector(vec);
    }

    Matrix3x3 Matrix3x3::RotationX(float angle)
    {
        return XMMatrixRotationX(angle);
    }

    Matrix3x3 Matrix3x3::RotationY(float angle)
    {
        return XMMatrixRotationY(angle);
    }

    Matrix3x3 Matrix3x3::RotationZ(float angle)
    {
        return XMMatrixRotationZ(angle);
    }

    Matrix3x3 Matrix3x3::RotationRollPitchYaw(const Vector3& vec)
    {
        return XMMatrixRotationRollPitchYaw(vec.x(), vec.y(), vec.z());
    }

    Matrix3x3 Matrix3x3::RotationQuaternion(const Quaternion& vec)
    {
        return XMMatrixRotationQuaternion(vec);
    }

    Matrix3x3 Matrix3x3::RotationNormal(const Vector3& axis, float angle)
    {
        return XMMatrixRotationNormal(axis, angle);
    }

    Matrix3x3 Matrix3x3::RotationAxis(const Vector3& axis, float angle)
    {
        return XMMatrixRotationAxis(axis, angle);
    }

    Matrix3x3 Matrix3x3::SR(const Matrix3x3& scale, const Matrix3x3& rotation)
    {
        return scale * rotation;
    }

    Matrix3x3 Matrix3x3::GetInverse() const
    {
        return XMMatrixInverse(nullptr, *this);
    }

    Matrix3x3 Matrix3x3::GetTranspose() const
    {
        return XMMatrixTranspose(*this);
    }

    Vector4 Matrix3x3::GetDeterminant() const
    {
        return XMMatrixDeterminant(*this);
    }

    std::pair<Quaternion, Vector3> Matrix3x3::GetRotationScale() const
    {
        XMVECTOR position, rotation, scale;
        XMMatrixDecompose(&scale, &rotation, &position, *this);
        return { rotation, scale };
    }

    void Matrix3x3::SetRotationScale(const Quaternion& rotation, const Vector3& scale)
    {
        *this = XMMatrixScalingFromVector(scale) *
                XMMatrixRotationQuaternion(rotation);
    }

    Vector3 Matrix3x3::GetScale() const
    {
        XMVECTOR scale, unused;
        XMMatrixDecompose(&scale, &unused, &unused, *this);
        return scale;
    }

    void Matrix3x3::SetScale(const Vector3& scale)
    {
        XMVECTOR rotation, unused;
        XMMatrixDecompose(&unused, &rotation, &unused, *this);
        *this = XMMatrixScalingFromVector(scale) *
                XMMatrixRotationQuaternion(rotation);
    }

    Quaternion Matrix3x3::GetRotationQuat() const
    {
        return XMQuaternionRotationMatrix(*this);
    }

    void Matrix3x3::SetRotationQuat(const Quaternion& quat)
    {
        XMVECTOR scale, unused;
        XMMatrixDecompose(&scale, &unused, &unused, *this);
        *this = XMMatrixRotationQuaternion(quat) *
                XMMatrixScalingFromVector(scale);
    }

    Matrix3x3 Matrix3x3::operator*(const Matrix3x3& other) const
    {
        return XMMatrixMultiply(*this, other);
    }

    Matrix3x3& Matrix3x3::operator*=(const Matrix3x3& other)
    {
        *this = XMMatrixMultiply(*this, other);
        return *this;
    }
} // namespace Ame::Math