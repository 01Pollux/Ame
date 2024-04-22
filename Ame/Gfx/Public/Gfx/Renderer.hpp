#pragma once

#include <Core/Ame.hpp>

namespace Ame::Rhi
{
    class Device;
} // namespace Ame::Rhi

namespace Ame::Gfx
{
    class Renderer
    {
    public:
        Renderer(
            std::reference_wrapper<Rhi::Device> Device);

        /// <summary>
        /// Update the renderer and all its components such as the camera, the scene, lights, etc.
        /// </summary>
        void Update(
            double DeltaTime);

        /// <summary>
        /// Render the scene to the screen
        /// </summary>
        void Render();

    private:
        std::reference_wrapper<Rhi::Device> m_Device;
    };
} // namespace Ame::Gfx