#pragma once

#include <RG/ResourceStorage.hpp>
#include <RG/DependencyLevel.hpp>

namespace Ame::RG
{
    class Context
    {
        friend class PassStorage;
        friend class Graph;

        using DependencyLevelListType = std::vector<DependencyLevel>;

    public:
        Context(
            Rhi::Device& Device);

    public:
        /// <summary>
        /// Get the render graph storage
        /// </summary>
        [[nodiscard]] const ResourceStorage& GetStorage() const;

        /// <summary>
        /// Get the render graph storage
        /// </summary>
        [[nodiscard]] ResourceStorage& GetStorage();

    private:
        /// <summary>
        /// Update core storage resources incase of changes
        /// </summary>
        void Update();

        /// <summary>
        /// Update the frame storage
        /// </summary>
        void UpdateFrameStorage(
            float                        engineTime,
            float                        gameTime,
            float                        deltaTime,
            const Ecs::Entity&           cameraEntity,
            const Math::TransformMatrix& transform,
            const Math::Matrix4x4&       projection,
            const Math::Vector2&         viewport);

        /// <summary>
        /// Execute the render graph
        /// </summary>
        void Execute();

        /// <summary>
        /// Build the render graph
        /// </summary>
        void Build(
            DependencyLevelListType&& levels);

    private:
        Ref<Rhi::Device>             m_Device;
        ResourceStorage              m_Resources;
        std::vector<DependencyLevel> m_Levels;
    };
} // namespace Ame::RG