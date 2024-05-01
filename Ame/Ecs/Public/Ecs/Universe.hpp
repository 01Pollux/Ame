#pragma once

#include <unordered_map>
#include <Ecs/World.hpp>
#include <Ecs/Signals/Universe.hpp>

#include <Core/Signals/Frame.hpp>

namespace Ame
{
    class IFrame;
    class FrameTimer;
} // namespace Ame

namespace Ame::Ecs
{
    class Universe
    {
        using WorldMap = std::unordered_map<StringU8, World>;

    public:
        Universe(
            IFrame&     Frame,
            FrameTimer& Timer);

    public:
        /// <summary>
        /// Add a world to the universe.
        /// </summary>
        World& CreateWorld(
            const StringU8& Name);

        /// <summary>
        /// Remove a world from the universe.
        /// </summary>
        void RemoveWorld(
            const StringU8& Name);

        /// <summary>
        /// Check if the universe has a world.
        /// </summary>
        bool HasWorld(
            const StringU8& Name);

        /// <summary>
        /// Get the world by name.
        /// </summary>
        [[nodiscard]] World& GetWorld(
            const StringU8& Name);

    public:
        /// <summary>
        /// Set the active world.
        /// If the world does not exist, it will be set to nullptr.
        /// </summary>
        void SetActiveWorld(
            World& EcsWorld);

        /// <summary>
        /// Get the active world.
        /// if the world does not exist, it will return nullptr.
        /// </summary>
        [[nodiscard]] const World* GetActiveWorld() const;

        /// <summary>
        /// Get the active world.
        /// if the world does not exist, it will return nullptr.
        /// </summary>
        [[nodiscard]] World* GetActiveWorld();

    public:
        AME_SIGNAL_DOUBLE(OnWorldChange);

    private:
        /// <summary>
        /// Progress current active worlds.
        /// </summary>
        void ProgressActiveWorld(
            double DeltaTime);

    private:
        Signals::OnUpdate::Handle m_OnUpdate;

        WorldMap m_Worlds;
        World*   m_ActiveWorld = nullptr;
    };
} // namespace Ame::Ecs