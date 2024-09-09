#pragma once

#include <Core/Interface.hpp>
#include <Core/Coroutine.hpp>

namespace Ame::Window
{
    /// <summary>
    /// IGlfwDriver is a class dedicated to managing and executing GLFW operations (glfw* functions).
    ///
    /// It is actually a wrapper around a single-threaded executor that is used to execute GLFW operations.
    /// Failure to use this class to execute GLFW operations will result in undefined behavior and possible deadlocks.
    /// </summary>
    class IGlfwDriver : public Co::derivable_executor<IGlfwDriver>
    {
    public:
        using Co::derivable_executor<IGlfwDriver>::derivable_executor;
    };
} // namespace Ame::Window
