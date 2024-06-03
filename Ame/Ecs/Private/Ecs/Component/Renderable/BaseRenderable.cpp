#include <Ecs/Component/Renderable/BaseRenderable.hpp>
#include <bit>

#include <Log/Wrapper.hpp>

namespace Ame::Ecs::Component
{
    auto BaseRenderable::BufferView::Local(
        const std::byte* data,
        size_t           count,
        size_t           stride) -> BufferView
    {
        return BufferView{
            .OffsetOrCpuData{ .CpuData = data },
            .Count  = static_cast<uint32_t>(count),
            .Stride = static_cast<uint32_t>(stride)
        };
    }

    auto BaseRenderable::BufferView::Shared(
        nri::Buffer* buffer,
        size_t       offset,
        size_t       count,
        size_t       stride) -> BufferView
    {
        return BufferView{
            .NriBuffer = buffer,
            .OffsetOrCpuData{ .Offset = offset },
            .Count  = static_cast<uint32_t>(count),
            .Stride = static_cast<uint32_t>(stride)
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
        return OffsetOrCpuData.Offset;
    }

    const std::byte* BaseRenderable::BufferView::CpuData() const
    {
// Reading from offset MUST means that we have a unique buffer
#ifdef AME_DEBUG
        if (!IsLocal())
        {
            Log::Ecs().Fatal("Trying to get offset from a shared buffer");
        }
#endif
        return OffsetOrCpuData.CpuData;
    }

    size_t BaseRenderable::BufferView::Size() const
    {
        return static_cast<size_t>(Count) * Stride;
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