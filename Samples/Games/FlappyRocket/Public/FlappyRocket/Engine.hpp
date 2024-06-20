#pragma once

#include <Engine/Engine.hpp>
#include <FlappyRocket/Game.hpp>

namespace Ame::FlappyRocket
{
    class FlappyRocketEngine : public IoCContainer
    {
    public:
        FlappyRocketEngine();

        Co::result<void> Run();

    protected:
        Co::null_result Initialize();

    private:
        void CreateWorld();

    private:
        UPtr<FlappyRocketGame> m_Game;
    };
} // namespace Ame::FlappyRocket