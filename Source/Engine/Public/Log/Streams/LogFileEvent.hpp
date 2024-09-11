#pragma once

#include <functional>

namespace Ame::Log
{
    struct LogFileEvent
    {
        using EmptyCallback = std::function<void(const String& fileName)>;
        using FileCallback  = std::function<void(const String& fileName, std::FILE* fileStream)>;

        EmptyCallback BeforeOpen;
        FileCallback  AfterOpen;
        FileCallback  BeforeClose;
        EmptyCallback AfterClose;
    };
} // namespace Ame::Log
