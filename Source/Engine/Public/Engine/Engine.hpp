#pragma once

#include <Core/Interface.hpp>

namespace Ame
{
    class AmeEngine
    {
    public:
        void Run();
    };
} // namespace Ame


using namespace Potto;

/// <summary>
/// Logger interface declared with potto macros.
/// This interface is equivalent to
// class ILogger : public PottoCom::IUnknown
// {
// public:
//	 virtual ~IFlyable() {}
//	 virtual bool Fly(int distance) = 0;
// };
/// </summary>
POTTO_INTERFACE_BEGINE("{d4c5a174-763c-4e6b-83f8-73355002fb7f}", IFlyable)
/// <summary>
/// Outputs Info message.
/// </summary>
POTTO_METHOD(bool, Fly(int distance))
POTTO_INTERFACE_END

/// <summary>
///
/// </summary>
POTTO_MULTITHREADED_OBJECT("0633e58e-11b0-4aa4-9ed7-bb147367fbe0", Sparrow)
POTTO_IMPLEMENT_INTERFACE(IFlyable)
{
public:
    POTTO_BEGIN_INTERFACE_MAP(Sparrow)
    POTTO_INTERFACE_ENTRY(IFlyable)
    POTTO_END_INTERFACE_MAP()

    Sparrow();

    bool Fly(int distance);
};

/// <summary>
///
/// </summary>
POTTO_CLASS_REGISTER(Sparrow);
