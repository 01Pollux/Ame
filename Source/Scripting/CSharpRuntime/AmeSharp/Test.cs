using AmeSharp.Core.Base;
using System.Runtime.InteropServices;

namespace AmeSharp
{
    public class Test
    {
        class MyObject : IDisposable
        {
            public MyObject() { }
            ~MyObject()
            {
                Console.WriteLine("Finalized");
            }

            public void Dispose()
            {
                Console.WriteLine("Disposed");
                GC.SuppressFinalize(this);
            }
        }

        public static void foo(IntPtr obj)
        {
            MyObject x = new();
            IBaseObject thisObj = new(obj);
            x = new();
        }

        [UnmanagedCallersOnly]
        public static void Run(IntPtr obj)
        {
            foo(obj);
        }
    }
}
