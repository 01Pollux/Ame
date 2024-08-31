#pragma once

#include <Core/Ame.hpp>
#include <Application/Config.hpp>

namespace Ame
{
    class AmeEngine;

    class BaseApplication
    {
    public:
        explicit BaseApplication(const ApplicationConfig& applicationConfig);

        BaseApplication(const BaseApplication&)            = delete;
        BaseApplication& operator=(const BaseApplication&) = delete;

        BaseApplication(BaseApplication&&)            = delete;
        BaseApplication& operator=(BaseApplication&&) = delete;

        virtual ~BaseApplication();

    public:
        int Run();

    protected:
        virtual void OnLoad()
        {
        }

        virtual void OnInitialize()
        {
        }

        virtual void OnShutdown()
        {
        }

        virtual void OnUnload()
        {
        }

    protected:
        [[nodiscard]] AmeEngine& GetEngine() noexcept
        {
            return *m_Engine;
        }

        [[nodiscard]] const AmeEngine& GetEngine() const noexcept
        {
            return *m_Engine;
        }

    private:
        UniquePtr<AmeEngine> m_Engine;
    };
} // namespace Ame
