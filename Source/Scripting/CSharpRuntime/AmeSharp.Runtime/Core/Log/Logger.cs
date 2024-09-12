using System.Runtime.InteropServices;

namespace AmeSharp.Runtime.Core.Log
{
    public class Logger
    {
        //[MethodImpl(MethodImplOptions.InternalCall)]
        //public static extern void LogMessage(LoggerInfo logInfo);

        [UnmanagedCallersOnly]
        public static void Test()
        {
            //LoggerInfo logInfo = new LoggerInfo
            //{
            //    Message = "Test",
            //    Level = LogLevel.Info
            //};
            ////LogMessage(logInfo);
            //Console.WriteLine(logInfo.Message);
            //Console.WriteLine(logInfo.Level);
            Console.WriteLine("))");
        }
    }
}
