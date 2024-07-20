#include <Core/Ame.Debug.hpp>
#ifndef AME_DIST

#include <stacktrace>
#include <iostream>

namespace Ame
{
    void DebugBreak()
    {
        std::cerr << std::stacktrace::current() << std::endl;
        DbgImpl::DebugBreak();
    }
} // namespace Ame
#endif
