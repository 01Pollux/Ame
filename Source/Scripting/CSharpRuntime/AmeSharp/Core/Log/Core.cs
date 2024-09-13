using AmeSharp.Core.Base;

namespace AmeSharp.Core.Log
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
