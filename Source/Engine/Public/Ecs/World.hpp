#pragma once

#include <Core/Interface.hpp>
#include <Core/Ame.hpp>

#include <Ecs/Entity.hpp>
#include <Ecs/Query.hpp>
#include <Ecs/System.hpp>
#include <Ecs/Observer.hpp>

namespace Ame::Rhi
{
    class IRhiDevice;
} // namespace Ame::Rhi

namespace Ame::Ecs
{
    // {FB443EB9-0F56-4640-B724-DE48E737BB9C}
    inline constexpr UId IID_EntityWorld{
        0xfb443eb9, 0xf56, 0x4640, { 0xb7, 0x24, 0xde, 0x48, 0xe7, 0x37, 0xbb, 0x9c }
    };

    class AME_ENGINE_API World : public BaseObject<IObject>
    {
    public:
        struct This
        {
            World* world;
        };

    public:
        using Base = BaseObject<IObject>;

        IMPLEMENT_QUERY_INTERFACE_IN_PLACE(IID_EntityWorld, Base);

    private:
        IMPLEMENT_INTERFACE_CTOR(World, Ptr<Rhi::IRhiDevice> rhiDevice);

    public:
        ~World() override;

    public:
        /// <summary>
        /// Create a new entity in the world.
        /// If the entity's name is already in use, Name will be modified to be unique using 'CreateUniqueEntityName'.
        /// </summary>
        Entity CreateEntity(StringView name = "", const Entity& parent = {});

        /// <summary>
        /// Get entity from its id.
        /// </summary>
        [[nodiscard]] Entity GetEntityById(const EntityId id) const;

    public:
        /// <summary>
        /// Create a new query in the world for querying entities.
        /// </summary>
        template<typename... ArgsTy> [[nodiscard]] QueryBuilder<ArgsTy...> CreateQuery()
        {
            return m_World->query_builder<ArgsTy...>();
        }

    public:
        /// <summary>
        /// Create a new system in the world for processing entities.
        /// </summary>
        template<typename... ArgsTy> [[nodiscard]] SystemBuilder<ArgsTy...> CreateSystem()
        {
            return m_World->system<ArgsTy...>();
        }

    public:
        /// <summary>
        /// Create a new observer in the world for tracking entities.
        /// </summary>
        template<typename... ArgsTy> [[nodiscard]] ObserverBuilder<ArgsTy...> CreateObserver()
        {
            return m_World->observer<ArgsTy...>();
        }

    public:
        [[nodiscard]] auto operator->() const noexcept
        {
            return m_World.get();
        }

        [[nodiscard]] auto& operator*()
        {
            return *m_World;
        }

        [[nodiscard]] auto GetFlecsWorld() const noexcept
        {
            return m_World.get();
        }

    public:
        /// <summary>
        /// Progress the world.
        /// </summary>
        /// <returns>True if the world is still running, false if the world has stopped.</returns>
        [[nodiscard]] bool Progress(double deltaTime);

    public:
        /// <summary>
        /// Register a new module in the world.
        /// </summary>
        template<typename Ty> auto ImportModule()
        {
            return m_World->import <Ty>();
        }

    public:
        [[nodiscard]] auto& GetRhiDevice() const noexcept
        {
            return m_RhiDevice;
        }

    private:
        UniquePtr<flecs::world> m_World;
        Ptr<Rhi::IRhiDevice>    m_RhiDevice;
    };

    //

    class WorldRef
    {
    public:
        WorldRef(World& world) : m_World(world->get_world())
        {
        }

        WorldRef(flecs::world world) : m_World(std::move(world))
        {
        }

    public:
        [[nodiscard]] World* GetWorld() noexcept
        {
            return m_World.get_mut<World::This>()->world;
        }

        [[nodiscard]] const World* GetWorld() const noexcept
        {
            return m_World.get<World::This>()->world;
        }

    public:
        /// <summary>
        /// Create a new entity in the world.
        /// If the entity's name is already in use, Name will be modified to be unique using 'CreateUniqueEntityName'.
        /// </summary>
        Entity CreateEntity(StringView name, const Entity& parent = {});

        /// <summary>
        /// Get entity from its id.
        /// </summary>
        [[nodiscard]] Entity GetEntityById(const EntityId id) const;

    public:
        /// <summary>
        /// Create a new query in the world for querying entities.
        /// </summary>
        template<typename... ArgsTy> [[nodiscard]] QueryBuilder<ArgsTy...> CreateQuery()
        {
            return m_World.query_builder<ArgsTy...>();
        }

    public:
        /// <summary>
        /// Create a new system in the world for processing entities.
        /// </summary>
        template<typename... ArgsTy> [[nodiscard]] SystemBuilder<ArgsTy...> CreateSystem()
        {
            return m_World.system<ArgsTy...>();
        }

    public:
        /// <summary>
        /// Create a new observer in the world for tracking entities.
        /// </summary>
        template<typename... ArgsTy> [[nodiscard]] ObserverBuilder<ArgsTy...> CreateObserver()
        {
            return m_World.observer<ArgsTy...>();
        }

    public:
        [[nodiscard]] auto operator->() const noexcept
        {
            return &m_World;
        }

        [[nodiscard]] auto& operator*() const noexcept
        {
            return m_World;
        }

        [[nodiscard]] auto& Get() const noexcept
        {
            return m_World;
        }

    public:
        [[nodiscard]] auto operator->()
        {
            return &m_World;
        }

        [[nodiscard]] auto& operator*()
        {
            return m_World;
        }

        [[nodiscard]] auto& Get()
        {
            return m_World;
        }

    public:
        /// <summary>
        /// Register a new module in the world.
        /// </summary>
        template<typename Ty> void ImportModule()
        {
            m_World.import <Ty>();
        }

    protected:
        flecs::world m_World;
    };

    //

    class AsyncWorld : public WorldRef
    {
    public:
        AsyncWorld(World& world) : WorldRef(world->async_stage())
        {
        }

        AsyncWorld(flecs::world world) : WorldRef(world.async_stage())
        {
        }

        void Merge()
        {
            m_World.merge();
        }
    };

    //

    class ScopedDeferredWorld : public WorldRef
    {
    public:
        ScopedDeferredWorld(World& world) : WorldRef(world)
        {
            m_World.defer_begin();
        }

        ScopedDeferredWorld(flecs::world world) : WorldRef(std::move(world))
        {
            m_World.defer_begin();
        }

        ScopedDeferredWorld(const ScopedDeferredWorld&) = delete;
        ScopedDeferredWorld(ScopedDeferredWorld&&)      = delete;

        ScopedDeferredWorld& operator=(const ScopedDeferredWorld&) = delete;
        ScopedDeferredWorld& operator=(ScopedDeferredWorld&&)      = delete;

        ~ScopedDeferredWorld()
        {
            m_World.defer_end();
        }
    };

    //

    class ScopedDeferredSuspensionWorld : public WorldRef
    {
    public:
        ScopedDeferredSuspensionWorld(World& world) : WorldRef(world)
        {
            m_World.defer_suspend();
        }

        ScopedDeferredSuspensionWorld(flecs::world world) : WorldRef(std::move(world))
        {
            m_World.defer_suspend();
        }

        ScopedDeferredSuspensionWorld(const ScopedDeferredSuspensionWorld&) = delete;
        ScopedDeferredSuspensionWorld(ScopedDeferredSuspensionWorld&&)      = delete;

        ScopedDeferredSuspensionWorld& operator=(const ScopedDeferredSuspensionWorld&) = delete;
        ScopedDeferredSuspensionWorld& operator=(ScopedDeferredSuspensionWorld&&)      = delete;

        ~ScopedDeferredSuspensionWorld()
        {
            m_World.defer_resume();
        }
    };

    //

    class ScopedAsyncWorld : public AsyncWorld
    {
    public:
        ScopedAsyncWorld(World& world) : AsyncWorld(world)
        {
            m_World.defer_begin();
        }

        ScopedAsyncWorld(flecs::world world) : AsyncWorld(std::move(world))
        {
            m_World.defer_begin();
        }

        ScopedAsyncWorld(const ScopedAsyncWorld&) = delete;
        ScopedAsyncWorld(ScopedAsyncWorld&&)      = delete;

        ScopedAsyncWorld& operator=(const ScopedAsyncWorld&) = delete;
        ScopedAsyncWorld& operator=(ScopedAsyncWorld&&)      = delete;

        ~ScopedAsyncWorld()
        {
            m_World.defer_end();
        }
    };
} // namespace Ame::Ecs