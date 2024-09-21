﻿using AmeSharp.Bridge.Core.Log;

namespace AmeSharp.Core.Log.Streams;

public class IFileLoggerStream(string filePath, bool truncate = false) :
    ILoggerStream(LoggerStreamBridge.CreateFile(filePath, truncate))
{
}
