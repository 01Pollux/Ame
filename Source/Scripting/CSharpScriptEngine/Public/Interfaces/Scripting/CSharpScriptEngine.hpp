#pragma once

#include <Interfaces/Scripting/ScriptEngine.hpp>
#include <CSharpScripting/Export.hpp>
#include <CSharpScripting/EngineConfig.hpp>

namespace Ame::Interfaces
{
    class IScriptEngine;

    [[nodiscard]] AME_CSHARP_API Ptr<IScriptEngine> CreateCSharpScriptingEngine(
        const Scripting::CSScriptEngineConfig& config);

    // {5545B2AF-9F71-4E34-B32A-677AB66D4849}
    inline constexpr UId IID_CSScriptEngine{
        0x5545b2af, 0x9f71, 0x4e34, { 0xb3, 0x2a, 0x67, 0x7a, 0xb6, 0x6d, 0x48, 0x49 }
    };
} // namespace Ame::Interfaces