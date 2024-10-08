#pragma once

#include <limits>
#include <Rhi/Core.hpp>

#include <EcsComponent/Math/Transform.hpp>
#include <EcsComponent/Renderables/BaseRenderable.hpp>

#include <Math/Camera.hpp>
#include <Ecs/World.hpp>
#include <Rg/Graph.hpp>

#include <Graphics/RenderGraph/Common/FrameConstants.hpp>

namespace Ame::Rhi
{
    class Material;
} // namespace Ame::Rhi

namespace Ame::Gfx
{
    class EntityStorage;
    class EntityCollector;

    //

    enum class EntityBoundingType : uint8_t
    {
        None,
        AABB,
        Sphere
    };

    struct EntityInstanceCode
    {
        EntityBoundingType BoundingType : 2 = EntityBoundingType::None;
    };

    struct EntityDrawInstance
    {
        uint32_t           TransformId = std::numeric_limits<uint32_t>::max();
        uint32_t           BoundingId  = std::numeric_limits<uint32_t>::max();
        EntityInstanceCode Code;
    };
    static_assert(sizeof(EntityDrawInstance) == Math::AlignUp(sizeof(EntityDrawInstance), 4),
                  "EntityDrawInstance size is not aligned to 4 bytes");

    enum class DrawInstanceType : uint8_t
    {
        Opaque,      // Opaque entities are rendered first with no blending
        Transparent, // Transparent entities are rendered second with blending
        Overlay,     // Overlay entities are rendered last with no depth testing
        Count
    };

    struct EntityDrawCommand
    {
        Ecs::IBaseRenderable* Renderable = nullptr;
        uint32_t              InstanceId = std::numeric_limits<uint32_t>::max();
        float                 Distance   = std::numeric_limits<float>::quiet_NaN();

        void SetDistance(const Math::Vector3& cameraPosition)
        {
            // if distance was not calculated yet
            if (Distance == Distance)
            {
                return;
            }

            // TODO: Implement distance calculation
            Distance = cameraPosition.DistanceTo({});
        }

        [[nodiscard]] auto operator<=>(const EntityDrawCommand& other) const noexcept
        {
            auto& renderableDescA = Renderable->GetRenderableDesc();
            auto& renderableDescB = other.Renderable->GetRenderableDesc();

            return std::tie(renderableDescA, Distance) <=> std::tie(renderableDescB, other.Distance);
        }
    };

    //

    using EntityDrawCommandList      = std::vector<EntityDrawCommand>;
    using EntityDrawCommandsCategory = std::array<EntityDrawCommandList, static_cast<size_t>(DrawInstanceType::Count)>;
} // namespace Ame::Gfx