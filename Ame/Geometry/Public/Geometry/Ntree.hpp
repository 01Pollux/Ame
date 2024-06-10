#pragma once

#include <Geometry/Common.hpp>
#include <Geometry/AABB.hpp>

#include <octree.h>

namespace OrthoTree::AmeAdapter
{
    template<dim_t DimensionCount, typename Ty = float>
    struct AdaptorGeneralBasics
    {
        using value_type  = Ty;
        using vector_type = Ame::Math::Impl::MVector<value_type, DimensionCount>;
        using box_type    = Ame::Geometry::AABB;
        using ray_type    = Ame::Math::Ray<vector_type>;
        using plane_type  = Ame::Math::Plane;

        static constexpr value_type GetPointC(const vector_type& point, dim_t dimensionID) noexcept
        {
            return point[dimensionID];
        }
        static constexpr void SetPointC(vector_type& point, dim_t dimensionID, value_type value) noexcept
        {
            point[dimensionID] = value;
        }

        static constexpr value_type Gebox_typeMinC(const box_type& box, dim_t dimensionID) noexcept
        {
            return GetPointC(box.Min(), dimensionID);
        }
        static constexpr value_type Gebox_typeMaxC(const box_type& box, dim_t dimensionID) noexcept
        {
            return GetPointC(box.Max(), dimensionID);
        }
        static constexpr void Sebox_typeMinC(box_type& box, dim_t dimensionID, value_type value) noexcept
        {
            // reconstruct the box (extents, center) from min
            auto min         = box.Min();
            min[dimensionID] = static_cast<value_type>(value);
            box              = Ame::Geometry::AABB::FromMinMax(min, box.Max());
        }
        static constexpr void Sebox_typeMaxC(box_type& box, dim_t dimensionID, value_type value) noexcept
        {
            // reconstruct the box (extents, center) from max
            auto max         = box.Max();
            max[dimensionID] = static_cast<value_type>(value);
            box              = Ame::Geometry::AABB::FromMinMax(box.Min(), max);
        }

        static constexpr const vector_type& Geray_typeDirection(const ray_type& ray) noexcept
        {
            return ray.Direction;
        }
        static constexpr const vector_type& Geray_typeOrigin(const ray_type& ray) noexcept
        {
            return ray.Origin;
        }

        static constexpr const vector_type& Geplane_typeNormal(const plane_type& plane) noexcept
        {
            return plane.GetNormal();
        }
        static constexpr value_type Geplane_typeOrigoDistance(const plane_type& plane) noexcept
        {
            return static_cast<value_type>(plane.GetDistance());
        }
    };

    template<dim_t DimensionCount, typename Ty = float>
    using AmeAdaptorGeneral = AdaptorGeneralBase<
        DimensionCount,
        AdaptorGeneralBasics<DimensionCount, Ty>::vector_type,
        AdaptorGeneralBasics<DimensionCount, Ty>::box_type,
        AdaptorGeneralBasics<DimensionCount, Ty>::ray_type,
        AdaptorGeneralBasics<DimensionCount, Ty>::plane_type,
        Ty,
        AdaptorGeneralBasics<DimensionCount, Ty>>;

    template<dim_t DimensionCount, typename Ty = float>
    using AmeOrthoTreePoint = OrthoTreePoint<
        DimensionCount,
        AdaptorGeneralBasics<DimensionCount, Ty>::vector_type,
        AdaptorGeneralBasics<DimensionCount, Ty>::box_type,
        AdaptorGeneralBasics<DimensionCount, Ty>::ray_type,
        AdaptorGeneralBasics<DimensionCount, Ty>::plane_type,
        Ty,
        AmeAdaptorGeneral<DimensionCount, Ty>>;

    template<typename DataTy, dim_t DimensionCount, typename Ty = float>
    using AmeOrthoTreePointContainer = OrthoTreeContainerPoint<
        AmeOrthoTreePoint<DimensionCount, Ty>,
        DataTy>;

    template<dim_t DimensionCount, uint32_t SplitDepthIncrement, typename Ty = float>
    using AmeOrthoTreeBoundingBox = OrthoTreeBoundingBox<
        DimensionCount,
        AdaptorGeneralBasics<3, float>::vector_type,
        AdaptorGeneralBasics<3, float>::box_type,
        AdaptorGeneralBasics<3, float>::ray_type,
        AdaptorGeneralBasics<3, float>::plane_type,
        float,
        SplitDepthIncrement,
        AmeAdaptorGeneral<3, float>>;

    template<typename DataTy, dim_t DimensionCount, uint32_t SplitDepthIncrement, typename Ty = float>
    using AmeOrthoTreeBoundingBoxContainer = OrthoTreeContainerBox<
        AmeOrthoTreeBoundingBox<DimensionCount, SplitDepthIncrement, Ty>,
        DataTy>;
} // namespace OrthoTree::AmeAdapter

namespace Ame::Geometry
{
    static constexpr uint32_t c_DefaultSplitDepthIncrement = 2;

    //

    template<int DimensionCount, typename Ty = float>
    using OrthoTreePoint = OrthoTree::AmeAdapter::AmeOrthoTreePoint<DimensionCount, Ty>;
    template<int DimensionCount, uint32_t SplitDepthIncrement = c_DefaultSplitDepthIncrement, typename Ty = float>
    using OrthoTreeBoundingBox = OrthoTree::AmeAdapter::AmeOrthoTreeBoundingBox<DimensionCount, SplitDepthIncrement, Ty>;

    template<typename DataTy, int DimensionCount, typename Ty = float>
    using OrthoTreePointContainer = OrthoTree::AmeAdapter::AmeOrthoTreePointContainer<DataTy, DimensionCount, Ty>;
    template<typename DataTy, int DimensionCount, uint32_t SplitDepthIncrement = c_DefaultSplitDepthIncrement, typename Ty = float>
    using OrthoTreeBoundingBoxContainer = OrthoTree::AmeAdapter::AmeOrthoTreeBoundingBoxContainer<DataTy, DimensionCount, SplitDepthIncrement, Ty>;

    //

    template<typename DataTy, int DimensionCount, typename Ty = float>
    struct OrthoTreePointT
    {
        using data_type                      = DataTy;
        static constexpr int dimension_count = DimensionCount;
        using scalar_type                    = Ty;

        using Container = OrthoTreePointContainer<DataTy, DimensionCount, Ty>;
        using Point     = OrthoTreePoint<DimensionCount, Ty>;
    };

    template<typename DataTy, int DimensionCount, uint32_t SplitDepthIncrement = c_DefaultSplitDepthIncrement, typename Ty = float>
    struct OrthoTreeBoxT
    {
        using data_type                                 = DataTy;
        static constexpr int      dimension_count       = DimensionCount;
        static constexpr uint32_t split_depth_increment = SplitDepthIncrement;
        using scalar_type                               = Ty;

        using Container = OrthoTreeBoundingBoxContainer<DataTy, DimensionCount, SplitDepthIncrement, Ty>;
        using Point     = OrthoTreeBoundingBox<DimensionCount, SplitDepthIncrement, Ty>;
    };

    //

    using QuadTreePoint  = OrthoTreePointT<Math::Vector2, 2, float>;
    using QuadTreePointI = OrthoTreePointT<Math::Vector2I, 2, int>;
    using QuadTreePointU = OrthoTreePointT<Math::Vector2U, 2, uint32_t>;

    using OctTreePoint  = OrthoTreePointT<Math::Vector3, 3, float>;
    using OctTreePointI = OrthoTreePointT<Math::Vector3I, 3, int>;
    using OctTreePointU = OrthoTreePointT<Math::Vector3U, 3, uint32_t>;

    using HexTreePoint  = OrthoTreePointT<Math::Vector4, 4, float>;
    using HexTreePointI = OrthoTreePointT<Math::Vector4I, 4, int>;
    using HexTreePointU = OrthoTreePointT<Math::Vector4U, 4, uint32_t>;

    //

    using QuadTreeBoundingBox  = OrthoTreeBoxT<AABB, 2, c_DefaultSplitDepthIncrement, float>;
    using QuadTreeBoundingBoxI = OrthoTreeBoxT<AABB, 2, c_DefaultSplitDepthIncrement, int>;
    using QuadTreeBoundingBoxU = OrthoTreeBoxT<AABB, 2, c_DefaultSplitDepthIncrement, uint32_t>;

    using OctTreeBoundingBox  = OrthoTreeBoxT<AABB, 3, c_DefaultSplitDepthIncrement, float>;
    using OctTreeBoundingBoxI = OrthoTreeBoxT<AABB, 3, c_DefaultSplitDepthIncrement, int>;
    using OctTreeBoundingBoxU = OrthoTreeBoxT<AABB, 3, c_DefaultSplitDepthIncrement, uint32_t>;

    using HexTreeBoundingBox  = OrthoTreeBoxT<AABB, 4, c_DefaultSplitDepthIncrement, float>;
    using HexTreeBoundingBoxI = OrthoTreeBoxT<AABB, 4, c_DefaultSplitDepthIncrement, int>;
    using HexTreeBoundingBoxU = OrthoTreeBoxT<AABB, 4, c_DefaultSplitDepthIncrement, uint32_t>;
} // namespace Ame::Geometry