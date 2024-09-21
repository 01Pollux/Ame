using AmeSharp.Bridge.Core.Base;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace AmeSharp.Core.Base;

[Guid("00000000-0000-0000-0000-000000000000")]
public class IBaseObject : INativeObject
{
    public IBaseObject(nint obj, bool isExternal) : base(obj)
    {
        if (!isExternal)
        {
            InitialzeCallbacks();
        }
    }
    public IBaseObject() : base(BaseObjectBridge.Create()) => InitialzeCallbacks();

    public IBaseObject? QueryInterface(ref Guid iid)
    {
        var output = BaseObjectBridge.QueryInterface(NativePointer, ref iid);
        if (output == nint.Zero)
        {
            return null;
        }
        return Get(output) as IBaseObject;
    }

    public T? QueryInterface<T>() where T : IBaseObject
    {
        Guid iid = typeof(T).GUID;
        var output = BaseObjectBridge.QueryInterface(NativePointer, ref iid);
        if (output == nint.Zero)
        {
            return null;
        }
        return Get(output) as T;
    }

    public static T? RequestInterface<T>(nint obj) where T : IBaseObject
    {
        Guid iid = typeof(T).GUID;
        var output = BaseObjectBridge.QueryInterface(obj, ref iid);
        if (output == nint.Zero)
        {
            return null;
        }
        return Get(output) as T;
    }

    //

    protected override void Dispose(bool disposing)
    {
        if (NativePointer != nint.Zero)
        {
            BaseObjectBridge.Release(NativePointer);
        }
        base.Dispose(disposing);
    }

    [UnmanagedCallersOnly(CallConvs = [typeof(CallConvCdecl)])]
    public static unsafe nint OnQueryInterface(nint _thisPointer, Guid* iid)
    {
        var @this = Get<IBaseObject>(_thisPointer)!;
        var obj = @this.QueryInterface(ref *iid);
        return obj is not null ? obj.NativePointer : nint.Zero;
    }

    private unsafe void InitialzeCallbacks()
    {
        BaseObjectBridge.SetQueryInterface(NativePointer, &OnQueryInterface);
    }
}
