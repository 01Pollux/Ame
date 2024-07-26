
#include <Geometry/FrustumPlanes.hpp>

namespace Ame::Geometry
{
    static Math::Vector4 CalculatePlane(
        Math::Vector4 vec)
    {
        return vec / Math::Vector3(vec).Length();
    }

    FrustumPlanes::FrustumPlanes(
        const Frustum& frustum) :
        Planes{
            CalculatePlane({ 0.0f, 0.0f, -1.0f, frustum.Near }),
            CalculatePlane({ 0.0f, 0.0f, 1.0f, -frustum.Far }),
            CalculatePlane({ 1.f, 0.0f, +frustum.RightSlope, 0.0f }),
            CalculatePlane({ -1.f, 0.0f, frustum.LeftSlope, 0.0f }),
            CalculatePlane({ 0.0f, 1.f, -frustum.TopSlope, 0.0f }),
            CalculatePlane({ 0.0f, -1.f, -frustum.BottomSlope, 0.0f })
        }
    {
    }
} // namespace Ame::Geometry