using AmeSharp.Bridge.Core;
using AmeSharp.Core.Base;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace AmeSharp.Core.Log
{
    public unsafe partial class LoggerEng
    {
        [LibraryImport(Libraries.AmeSharpRuntime, EntryPoint = "AmeCSharp_LogMessage")]
        [UnmanagedCallConv(CallConvs = [typeof(CallConvCdecl)])]
        public static partial void LogMessage(NativeString message, LogLevel level);
    }

    public unsafe partial class Logger
    {
        [UnmanagedCallersOnly]
        public static void Test()
        {
            Console.WriteLine("Test");
            LoggerEng.LogMessage("test", LogLevel.Info);
        }
    }
}
