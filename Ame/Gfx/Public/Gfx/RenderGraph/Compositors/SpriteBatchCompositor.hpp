#pragma once

#include <Gfx/Compositor/Signals.hpp>

namespace Ame::Gfx
{
    class SpriteBatchCompositor
    {
    public:
        class SpriteBatchCompositor(
            EntityCompositor& entityCompositor);

    private:
        void OnRenderCompose(
            Signals::Data::DrawCompositorData& renderData);

    private:
        Signals::ScopedConnection m_OnRenderCompose;
    };
} // namespace Ame::Gfx