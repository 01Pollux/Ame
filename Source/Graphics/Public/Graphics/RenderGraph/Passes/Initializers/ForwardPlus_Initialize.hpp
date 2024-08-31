#pragma once

#include <Rg/Pass.hpp>
#include <Graphics/RenderGraph/Common/Names.hpp>

namespace Ame::Gfx
{
    class ForwardPlus_InitializePass : public Rg::Pass
    {
    public:
        struct PassData
        {
            Rg::ResourceViewId FinalImageViewId;
        };

    public:
        ForwardPlus_InitializePass();

    private:
        void OnBuild(Rg::Resolver& resolver);

        void OnExecute(const Rg::ResourceStorage& storage, Dg::IDeviceContext* deviceContext);

    private:
        PassData m_PassData;
    };
} // namespace Ame::Gfx