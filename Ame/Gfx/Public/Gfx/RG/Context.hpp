#pragma once

#include <Gfx/RG/ResourceStorage.hpp>
#include <Gfx/RG/DependencyLevel.hpp>

namespace Ame::Gfx::RG
{
    class Context
    {
        friend class PassStorage;
        friend class Graph;

        using DependencyLevelListType = std::vector<DependencyLevel>;

    public:
        Context(
            Rhi::Device&   Device,
            Ecs::Universe& Universe);

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
        /// Reset camera storage resources
        /// </summary>
        void Reset();

        /// <summary>
        /// Update core storage resources incase of changes
        /// </summary>
        void Update();

        /// <summary>
        /// Update the frame storage
        /// </summary>
        void UpdateFrameStorage(
            float                        EngineTime,
            float                        GameTime,
            float                        DeltaTime,
            const Ecs::Entity&           CameraEntity,
            const Math::TransformMatrix& Transform,
            const Math::Matrix4x4&       Projection,
            const Math::Vector2&         Viewport);

        /// <summary>
        /// Execute the render graph
        /// </summary>
        void Execute();

        /// <summary>
        /// Build the render graph
        /// </summary>
        void Build(
            DependencyLevelListType&& Levels);

    private:
        Ref<Rhi::Device>             m_Device;
        ResourceStorage              m_Resources;
        std::vector<DependencyLevel> m_Levels;
    };
} // namespace Ame::Gfx::RG