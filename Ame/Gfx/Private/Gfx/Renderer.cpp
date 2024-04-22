#include <Gfx/Renderer.hpp>

namespace Ame::Gfx
{
    Renderer::Renderer(
        std::reference_wrapper<Rhi::Device> Device) :
        m_Device(std::move(Device))
    {
    }

    void Renderer::Update(
        double DeltaTime)
    {
    }

    void Renderer::Render()
    {
    }
} // namespace Ame::Gfx