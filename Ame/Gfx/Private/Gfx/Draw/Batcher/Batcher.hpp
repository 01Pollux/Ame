#pragma once

#include <Gfx/Draw/SpriteBatch/SpriteBatch.hpp>

namespace Ame::Gfx
{
    class DrawBatcher
    {
    public:
        void DrawSprite();

    private:
        SpriteBatch m_SpriteBatch;
    };
} // namespace Ame::Gfx