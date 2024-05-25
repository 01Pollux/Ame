#include <Ecs/Component/Renderable/BaseRenderable.hpp>
#include <bit>

#include <Log/Wrapper.hpp>

namespace Ame::Ecs::Component
{
    auto BaseRenderable::BufferView::Local(
        void*  data,
        size_t count,
        size_t stride) -> BufferView
    {
        return BufferView{
            .CpuViewOrOffset = std::bit_cast<const void*>(data),
            .Count             = static_cast<uint32_t>(count),
            .Stride            = static_cast<uint32_t>(stride)
        };
    }

    auto BaseRenderable::BufferView::Shared(
        nri::Buffer* buffer,
        size_t       offset,
        size_t       count,
        size_t       stride) -> BufferView
    {
        return BufferView{
            .NriBuffer         = buffer,
            .CpuViewOrOffset = std::bit_cast<const void*>(offset),
            .Count             = static_cast<uint32_t>(count),
            .Stride            = static_cast<uint32_t>(stride)
        };
    }

    //

    size_t BaseRenderable::BufferView::Offset() const
    {
// Reading from offset MUST means that we have a unique buffer
#ifdef AME_DEBUG
        if (!HasUniqueBuffer())
        {
            Log::Ecs().Fatal("Trying to get offset from a local buffer");
        }
#endif
        return std::bit_cast<uint64_t>(CpuViewOrOffset);
    }

    const void* BaseRenderable::BufferView::CpuData() const
    {
// Reading from offset MUST means that we have a unique buffer
#ifdef AME_DEBUG
        if (!IsLocal())
        {
            Log::Ecs().Fatal("Trying to get offset from a shared buffer");
        }
#endif
        return CpuViewOrOffset;
    }

    bool BaseRenderable::BufferView::HasUniqueBuffer() const
    {
        return NriBuffer != nullptr;
    }

    bool BaseRenderable::BufferView::IsLocal() const
    {
        return !HasUniqueBuffer();
    }
} // namespace Ame::Ecs::Component