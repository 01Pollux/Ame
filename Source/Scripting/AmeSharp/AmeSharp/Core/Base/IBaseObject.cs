using AmeSharp.Bridge.Core.Base;
using System.Runtime.InteropServices;

namespace AmeSharp.Core.Base;

[Guid("00000000-0000-0000-0000-000000000000")]
public class IBaseObject(IntPtr obj) : INativeObject(obj)
{
    public override nint NativePointer
    {
        protected set
        {
            base.NativePointer = value;
            if (NativePointer != IntPtr.Zero)
            {
                BaseObjectBridge.AddRef(NativePointer);
            }
        }
    }

    //

    public IBaseObject? QueryInterface(ref Guid iid)
    {
        var output = BaseObjectBridge.QueryInterface(NativePointer, ref iid);
        return output == IntPtr.Zero ? null : new IBaseObject(output);
    }

    public T? QueryInterface<T>() where T : IBaseObject
    {
        Guid iid = typeof(T).GUID;
        return QueryInterface(ref iid) as T;
    }

    //

    protected override void Dispose(bool disposing)
    {
        if (NativePointer != IntPtr.Zero)
        {
            BaseObjectBridge.Release(NativePointer);
        }
        base.Dispose(disposing);
    }
}
