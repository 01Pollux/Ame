using AmeSharp.Bridge.Core.Base;
using System.Runtime.InteropServices;

namespace AmeSharp.Core.Base
{
    [Guid("00000000-0000-0000-0000-000000000000")]
    public class IBaseObject : IDisposable
    {
        public IntPtr NativePointer { get; private set; }

        public IBaseObject(IntPtr obj)
        {
            NativePointer = obj;
            BaseObjectBridge.AddRef(NativePointer);
        }
        ~IBaseObject()
        {
            Dispose(false);
        }

        //

        public IBaseObject? QueryInterface(ref Guid iid)
        {
            IntPtr output = IntPtr.Zero;
            BaseObjectBridge.QueryInterface(NativePointer, ref iid, ref output);
            return output == IntPtr.Zero ? null : new IBaseObject(output);
        }

        public T? QueryInterface<T>() where T : IBaseObject
        {
            Guid iid = typeof(T).GUID;
            return QueryInterface(ref iid) as T;
        }

        //

        public void Dispose()
        {
            Dispose(true);
            GC.SuppressFinalize(this);
        }

        protected virtual void Dispose(bool _)
        {
            if (NativePointer != IntPtr.Zero)
            {
                BaseObjectBridge.Release(NativePointer);
                NativePointer = IntPtr.Zero;
            }
        }
    }
}
