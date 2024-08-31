#include <Interfaces/Graphics/RendererImpl.hpp>

#include <imgui.h>

namespace Ame::Interfaces
{
    Ptr<IRenderer> CreateRenderer(const RendererCreateDesc& createDesc)
    {
        return AmeCreate(RendererImpl, createDesc.RhiDevice, createDesc.World, createDesc.ImguiRenderer);
    }
} // namespace Ame::Interfaces
