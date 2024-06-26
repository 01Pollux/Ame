#pragma once

#include <Engine/Engine.hpp>
#include <FlappyRocket/Game.hpp>

namespace Ame::FlappyRocket
{
    class FlappyRocketEngine : public BaseEngine
    {
    public:
        FlappyRocketEngine();

    protected:
        void Initialize() override;

    private:
        void SetClearColor(
            Rhi::Device& rhiDevice) const;

    private:
        void CreateWorld();

    private:
        UPtr<FlappyRocketGame> m_Game;
    };
} // namespace Ame::FlappyRocket