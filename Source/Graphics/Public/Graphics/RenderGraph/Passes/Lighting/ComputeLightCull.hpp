#pragma once

#include <Rg/Pass.hpp>
#include <Graphics/RenderGraph/Common/Names.hpp>
#include <Shading/Technique.hpp>

namespace Ame::Ecs
{
    class World;
} // namespace Ame::Ecs

namespace Ame::Gfx
{
    class ComputeLightCullPass : public Rg::Pass
    {
    private:
        static constexpr uint32_t c_AverageOverlappingLightsPerTile = 200u;

        static constexpr const char c_LightIndices[] = "AllLightIndices";
        static constexpr const char c_DepthView[]    = "DepthView";

        static constexpr const char c_LightIndices_Transparent[] = "LightIndices_Transparent";
        static constexpr const char c_LightIndices_Opaque[]      = "LightIndices_Opaque";

        static constexpr const char c_LightHeads_Transparent[] = "LightHeads_Transparent";
        static constexpr const char c_LightHeads_Opaque[]      = "LightHeads_Opaque";

        static constexpr const char c_LightDebugTexture[] = "DebugTexture";

        struct PassData
        {
            Dg::IBufferView*  LightIds  = nullptr;
            Dg::ITextureView* DepthView = nullptr;

            Dg::IBufferView* LightIndices_Transparent = nullptr;
            Dg::IBufferView* LightIndices_Opaque      = nullptr;

            Dg::ITextureView* LightHeads_Transparent = nullptr;
            Dg::ITextureView* LightHeads_Opaque      = nullptr;

#ifndef AME_DIST
            Dg::ITextureView* DebugTexture = nullptr;
#endif

            Dg::IShaderResourceBinding* Srbs[2]{}; // _FRS_Graphics, _ERS_Graphics
        };

        struct ShaderData
        {
        };

    public:
        ComputeLightCullPass();

        void SetGridSize(
            uint8_t blockSize)
        {
            m_BlockSize = blockSize;
        }

        [[nodiscard]] uint8_t GetBlockSize() const
        {
            return m_BlockSize;
        }

    private:
        void TryCreateResources(
            Rhi::IRhiDevice* rhiDevice);

        void OnBuild(
            Rg::Resolver& resolver);

        void OnExecute(
            const Rg::ResourceStorage& storage,
            Dg::IDeviceContext*        deviceContext);

    private:
        PassData m_PassData;

        Ptr<Dg::IPipelineState>         m_PipelineState;
        Ptr<Dg::IShaderResourceBinding> m_Srb;

        Math::Vector2U m_DispatchSize;
        uint8_t        m_BlockSize = 16;
    };
} // namespace Ame::Gfx