using AmeSharp.Core.Base.Types;
using AmeSharp.Core.Marshallers.Log;
using System.Runtime.InteropServices.Marshalling;

namespace AmeSharp.Core.Log;

[NativeMarshalling(typeof(LoggerInfoMarshaller))]
public struct LoggerInfo
{
    public NativeStringView Message;
    public LogLevel Level;
}

