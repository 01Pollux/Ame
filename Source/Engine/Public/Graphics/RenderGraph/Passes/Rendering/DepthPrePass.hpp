#pragma once

#include <Rg/Pass.hpp>
#include <Graphics/RenderGraph/Common/Names.hpp>
#include <Shading/Technique.hpp>
#include <Graphics/RenderGraph/Passes/Helpers/StdRenderObjects.hpp>

namespace Ame::Ecs
{
    class World;
} // namespace Ame::Ecs

namespace Ame::Gfx
{
    class DepthPrePass : public Rg::Pass
    {
    public:
        static constexpr Dg::TEXTURE_FORMAT DepthFormat = Dg::TEX_FORMAT_D32_FLOAT;

        struct PassData
        {
            Rg::ResourceViewId          DepthView;
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
        DepthPrePass(Ecs::World* world);

    private:
        void BindResourcesOnce(const Rg::ResourceStorage& storage);
        void CreateResourcesOnce(const Rg::ResourceStorage& storage);

    private:
        void OnBuild(Rg::Resolver& resolver);

        void OnExecute(const Rg::ResourceStorage& storage, Dg::IDeviceContext* deviceContext);

    private:
        Ecs::World*                 m_World;
        PassData                    m_PassData;
        Ptr<Rhi::MaterialTechnique> m_Technique;
    };
} // namespace Ame::Gfx