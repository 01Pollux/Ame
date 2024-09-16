using AmeSharp.Bridge.Core.Log.Marshallers;
using AmeSharp.Core.Base.Types;
using System.Runtime.InteropServices.Marshalling;

namespace AmeSharp.Core.Log;

public enum LogLevel : byte
{
    Trace,
    Debug,
    Info,
    Warning,
    Error,
    Fatal,
    Disabled,
}

[NativeMarshalling(typeof(LoggerInfoMarshaller))]
public struct LoggerInfo
{
    public NativeStringView Message;
    public LogLevel Level;
}
