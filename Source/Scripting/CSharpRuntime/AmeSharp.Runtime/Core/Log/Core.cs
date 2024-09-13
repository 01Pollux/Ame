using AmeSharp.Runtime.Core.Base;

namespace AmeSharp.Runtime.Core.Log
{
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

    public struct LoggerInfo
    {
        public NativeString Message;
        public LogLevel Level;
    }
}
