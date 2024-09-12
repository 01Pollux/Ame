using System.Runtime.CompilerServices;

namespace AmeSharp.Runtime.Ame.Core.Log
{
    public class Logger
    {
        [MethodImpl(MethodImplOptions.InternalCall)]
        public static extern void LogMessage(LoggerInfo logInfo);
    }
}
