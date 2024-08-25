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
            Rg::ResourceViewId LightIds;
            Rg::ResourceViewId DepthView;

            Rg::ResourceViewId LightIndices_Transparent;
            Rg::ResourceViewId LightIndices_Opaque;

            Rg::ResourceViewId LightHeads_Transparent;
            Rg::ResourceViewId LightHeads_Opaque;

#ifndef AME_DIST
            Rg::ResourceViewId DebugTexture;
#endif

            Dg::IShaderResourceBinding* Srbs[2]{}; // _FRS_Graphics, _ERS_Graphics

            void Reset()
            {
                Srbs[0] = nullptr;
            }

            operator bool() const
            {
                return Srbs[0] != nullptr;
            }
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
        void BindResourcesOnce(const Rg::ResourceStorage& storage);
        void CreateResourcesOnce(const Rg::ResourceStorage& storage);

    private:
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