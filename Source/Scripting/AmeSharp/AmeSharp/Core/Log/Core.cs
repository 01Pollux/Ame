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
        public string Message;
        public LogLevel Level;
    }
}
