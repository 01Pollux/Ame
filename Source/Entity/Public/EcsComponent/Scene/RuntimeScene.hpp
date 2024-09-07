#pragma once

#include <EcsComponent/Core.hpp>
#include <Ecs/World.hpp>

namespace Ame::Ecs
{
    // {B1E66472-0485-4A63-9646-5486139D0270}
    inline constexpr UId IID_RuntimeScene{
        0xb1e66472, 0x485, 0x4a63, { 0x96, 0x46, 0x54, 0x86, 0x13, 0x9d, 0x2, 0x70 }
    };

    enum class RuntimeSceneMerge
    {
        /// <summary>
        /// Copy the 'this' scene's entity tree to the target scene root
        /// </summary>
        Copy,

        /// <summary>
        /// Move the 'this' scene's entity tree to the target scene root, the 'this' scene will be empty, if the
        /// current scene is same as 'this', no operation will be performed
        /// </summary>
        Move,

        /// <summary>
        /// Replace the target scene root with the 'this' scene's entity tree, if the target scene is same as 'this',
        /// no operation will be performed
        /// </summary>
        Replace,

        /// <summary>
        /// Clear the 'this' scene's entity tree
        /// </summary>
        Clear
    };

    class RuntimeScene : public BaseObject<ISerializable>
    {
    public:
        using Base = BaseObject<ISerializable>;
        IMPLEMENT_QUERY_INTERFACE_IN_PLACE(IID_RuntimeScene, Base);

    private:
        IMPLEMENT_INTERFACE_CTOR(RuntimeScene, Ecs::World* world);

    public:
        Ecs::Entity CloneEntity(const Ecs::Entity& entity);
        void        MergeTo(RuntimeScene* targetScene, RuntimeSceneMerge mergeType);

        [[nodiscard]] const Ecs::Entity& GetRoot() const noexcept
        {
            return m_Root;
        }
        [[nodiscard]] const flecs::entity& GetRootFlecsEntity() const noexcept
        {
            return m_Root->GetFlecsEntity();
        }

    private:
        Ptr<Ecs::World>   m_World;
        Ecs::UniqueEntity m_Root;
    };

    //

    AME_ECS_WRAP_COMPONENT_PTR(RuntimeScene);

    /// <summary>
    /// pair: [SceneEntityComponent, Entity] # entity
    /// Mark the entity as part of a scene
    /// </summary>
    struct SceneEntityPairComponent
    {
    };
} // namespace Ame::Ecs