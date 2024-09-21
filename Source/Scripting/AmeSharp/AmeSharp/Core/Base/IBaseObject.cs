using AmeSharp.Bridge.Core.Base;
using System.Runtime.InteropServices;

namespace AmeSharp.Core.Base;

[Guid("00000000-0000-0000-0000-000000000000")]
public class IBaseObject : INativeObject
{
    public IBaseObject(IntPtr obj) : base(obj) { }
    public IBaseObject() : base(BaseObjectBridge.Create()) { }

    public IBaseObject? QueryInterface(ref Guid iid)
    {
        var output = BaseObjectBridge.QueryInterface(NativePointer, ref iid);
        if (output == IntPtr.Zero)
        {
            return null;
        }
        return Get(output) as IBaseObject;
    }

    public T? QueryInterface<T>() where T : IBaseObject
    {
        Guid iid = typeof(T).GUID;
        var output = BaseObjectBridge.QueryInterface(NativePointer, ref iid);
        if (output == IntPtr.Zero)
        {
            return null;
        }
        return Get(output) as T;
    }

    public static T? RequestInterface<T>(IntPtr obj) where T : IBaseObject
    {
        Guid iid = typeof(T).GUID;
        var output = BaseObjectBridge.QueryInterface(obj, ref iid);
        if (output == IntPtr.Zero)
        {
            return null;
        }
        return Get(output) as T;
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
