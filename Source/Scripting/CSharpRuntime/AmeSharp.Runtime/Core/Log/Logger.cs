using AmeSharp.Runtime.Core.Base;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace AmeSharp.Runtime.Core.Log
{
    public unsafe partial class LoggerEng
    {
        [LibraryImport("AmeSharp", EntryPoint = "AmeCSharp_LogMessage")]
        [UnmanagedCallConv(CallConvs = [typeof(CallConvCdecl)])]
        public static partial void LogMessage(NativeString message, LogLevel level);
    }

    public unsafe partial class Logger
    {
        //[DllImport("AmeEngine.dll", EntryPoint = "AmeCSharp_LogMessage2", CallingConvention = CallingConvention.Cdecl)]
        //public static extern void LogMessage2(LoggerInfo logInfo);

        [UnmanagedCallersOnly]
        public static void Test()
        {
            Console.WriteLine("Test");
            LoggerEng.LogMessage("test", LogLevel.Info);
        }
    }
}
