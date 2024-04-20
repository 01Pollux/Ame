#pragma once

#include <Engine/Engine.hpp>
#include <Engine/Timer.hpp>

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
            Rhi::Device& Device) const;

    private:
        void CreateWorld();

    private:
        void LoopUpdate(
            const EngineTimer& Timer);

    private:
        FlappyRocketGame m_Game;
    };
} // namespace Ame::FlappyRocket