#include <Math/Plane.hpp>

namespace Ame::Math
{
    using namespace Util;

    Plane::Plane(const Vector3& normal, float distance) : Vector4(normal.x(), normal.y(), normal.z(), distance)
    {
    }

    Plane::Plane(const Vector3& point, Vector3 normal) : Plane(XMPlaneFromPointNormal(point, normal))
    {
    }

    Plane::Plane(const Vector3& a, const Vector3& b, const Vector3& c) : Plane(XMPlaneFromPoints(a, b, c))
    {
    }

    //

    void Plane::Normalize()
    {
        *this = XMPlaneNormalize(*this);
    }

    void Plane::NormalizeEst()
    {
        *this = XMPlaneNormalizeEst(*this);
    }

    //

    Opt<Vector3> Plane::IntersectLine(const Vector3& a, const Vector3& b) const
    {
        auto point = XMPlaneIntersectLine(*this, a, b);
        return XMVector3IsNaN(point) ? std::nullopt : Opt<Vector3>(point);
    }

    Opt<std::pair<Vector3, Vector3>> Plane::IntersectPlane(const Plane& other) const
    {
        XMVECTOR a, b;
        XMPlaneIntersectPlane(&a, &b, *this, other);
        return XMVector3IsNaN(a) ? std::nullopt : Opt(std::pair{ Vector3(a), Vector3(b) });
    }
} // namespace Ame::Math
