#pragma once

#include <Interfaces/Scripting/ScriptEngine.hpp>
#include <CSharpScripting/Export.hpp>

namespace Ame::Interfaces
{
    AME_CSHARP_API Ptr<IScriptEngine> CreateCSharpScriptingEngine();
} // namespace Ame::Interfaces