#pragma once

#include <Core/Interface.hpp>
#include <Core/Ame.hpp>

namespace Ame
{
    class AmeEngine;

    class BareApplication
    {
    public:
        BareApplication();

        virtual ~BareApplication() = default;

    public:
        int Run();

    protected:
        virtual void OnLoad()
        {
        }

        virtual void OnInitialize()
        {
        }

        virtual void Shutdown()
        {
        }

        virtual void OnUnload()
        {
        }

    private:
        UniquePtr<AmeEngine> m_Engine;
    };
} // namespace Ame

#define AME_APPLICATION_MAIN(ApplicationClass) \
	int main(int argc, char** argv) \
	{ \
		ApplicationClass app; \
		return app.Run(); \
	}
