#pragma once

namespace Ame::RG
{
    class Graph;
}

namespace Ame::Gfx
{
    class IBaseGraphPipeline
    {
    public:
        IBaseGraphPipeline() = default;

        IBaseGraphPipeline(const IBaseGraphPipeline&) = delete;
        IBaseGraphPipeline(IBaseGraphPipeline&&)      = delete;

        IBaseGraphPipeline& operator=(const IBaseGraphPipeline&) = delete;
        IBaseGraphPipeline& operator=(IBaseGraphPipeline&&)      = delete;

        virtual ~IBaseGraphPipeline() = default;
    };
} // namespace Ame::Gfx