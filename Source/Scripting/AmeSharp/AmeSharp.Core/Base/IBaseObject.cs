using AmeSharp.Bridge.Core.Base;
using System.Runtime.InteropServices;

namespace AmeSharp.Core.Base;

[Guid("00000000-0000-0000-0000-000000000000")]
public abstract partial class IBaseObject : INativeObject
{
    public static IBaseObject References(IntPtr handle) => new BaseObjectRef(handle);

    public IBaseObject(IntPtr handle, bool ownsHandle, bool installCallbacks) : base(handle, ownsHandle)
    {
        if (ownsHandle)
        {
            BaseObjectBridge.AddRef(handle);
        }
        if (installCallbacks)
        {
            InitializeCallbacks();
        }
    }

    public IBaseObject? QueryInterface(Guid iid)
    {
        var result = BaseObjectBridge.QueryInterface(Handle, iid);
        return IsHandleInvalid(result) ? null : References(result);
    }

    public virtual IBaseObject? OnQueryInterface(Guid iid) => null;

    //

    protected override bool ReleaseHandle()
    {
        BaseObjectBridge.Release(handle);
        return true;
    }

}
