#pragma once

#include <Core/Interface.hpp>
#include <Core/Ame.hpp>

namespace Ame
{
    class AmeEngine;

    class BareApplication
    {
    public:
        BareApplication(
            int    argc,
            char** argv);

        BareApplication(const BareApplication&)            = delete;
        BareApplication& operator=(const BareApplication&) = delete;

        BareApplication(BareApplication&&)            = delete;
        BareApplication& operator=(BareApplication&&) = delete;

        virtual ~BareApplication();

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

#define AME_APPLICATION_MAIN(ApplicationClass) \
    int main(int argc, char** argv)            \
    {                                          \
        ApplicationClass app;                  \
        return app.Run();                      \
    }
