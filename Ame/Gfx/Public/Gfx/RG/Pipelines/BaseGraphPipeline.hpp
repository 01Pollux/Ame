#pragma once

namespace Ame::Gfx::RG
{
    class Graph;

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
} // namespace Ame::Gfx::RG