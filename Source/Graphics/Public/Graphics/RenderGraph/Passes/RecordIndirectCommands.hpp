#pragma once

#include <RG/Pass.hpp>
#include <Graphics/RenderGraph/Common/Names.hpp>

namespace Ame::Ecs
{
    class World;
} // namespace Ame::Ecs

namespace Ame::Gfx
{
    class RecordIndirectCommandsPass : public RG::Pass
    {
        struct DispatchConstants
        {
            uint32_t FirstInstance;
            uint32_t InstanceCount;
            uint32_t CounterOffset;
        };

    public:
        static inline const RG::ResourceViewId RGRenderInstances       = c_RGRenderInstances("Record Indirect Commands");
        static inline const RG::ResourceViewId RGSortedRenderInstances = c_RGSortedRenderInstances("Record Indirect Commands");
        static inline const RG::ResourceViewId RGDrawCommands          = c_RGDrawCommands("Record Indirect Commands");
        static inline const RG::ResourceViewId RGDrawCommandCounts     = c_RGDrawCommandCounts("Record Indirect Commands");

    public:
        RecordIndirectCommandsPass(
            Ecs::World*        world);

    private:
        void CreateResources(
            Dg::IRenderDevice* renderDevice);

        void Build(
            RG::Resolver& resolver);

        void Execute(
            const RG::ResourceStorage& storage,
            Dg::IDeviceContext*        deviceContext);

    private:
        Ptr<Ecs::World> m_World;

        Ptr<Dg::IPipelineState>         m_PipelineState;
        Ptr<Dg::IShaderResourceBinding> m_Srb;
        Ptr<Dg::IBuffer>                m_DispatchConstants;
    };
} // namespace Ame::Gfx