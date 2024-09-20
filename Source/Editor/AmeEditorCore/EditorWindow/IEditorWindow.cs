using AmeEditorCore.Bridge.EditorWindow;
using AmeSharp.Core.Base;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace AmeEditorCore.EditorWindow;

public class IEditorWindow : IBaseObject
{
    [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
    private delegate void EditorWindowCallbacks();

    private GCHandle _thisHandle;

    public IEditorWindow(IntPtr obj) : base(obj)
    {
        InitialzeCallbacks();
    }

    public IEditorWindow(string path) : base(EditorWindowBridge.Create(path))
    {
        InitialzeCallbacks();
    }

    public string FullPath => EditorWindowBridge.GetFullPath(NativePointer).ToString();

    public virtual void OnDrawVisible() { }
    public virtual void OnToolbarDraw() { }
    public virtual void OnShow() { }
    public virtual void OnHide() { }

    //

    [UnmanagedCallersOnly(CallConvs = [typeof(CallConvCdecl)])]
    public static void OnDrawVisibleCallback(IntPtr thisObject)
    {
        var @this = GCHandle.FromIntPtr(thisObject).Target as IEditorWindow;
        @this!.OnDrawVisible();
    }
    [UnmanagedCallersOnly(CallConvs = [typeof(CallConvCdecl)])]
    public static void OnToolbarDrawCallback(IntPtr thisObject)
    {
        var @this = GCHandle.FromIntPtr(thisObject).Target as IEditorWindow;
        @this!.OnToolbarDraw();
    }
    [UnmanagedCallersOnly(CallConvs = [typeof(CallConvCdecl)])]
    public static void OnShowCallback(IntPtr thisObject)
    {
        var @this = GCHandle.FromIntPtr(thisObject).Target as IEditorWindow;
        @this!.OnShow();
    }
    [UnmanagedCallersOnly(CallConvs = [typeof(CallConvCdecl)])]
    public static void OnHideCallback(IntPtr thisObject)
    {
        var @this = GCHandle.FromIntPtr(thisObject).Target as IEditorWindow;
        @this!.OnHide();
    }

    private unsafe void InitialzeCallbacks()
    {
        _thisHandle = GCHandle.Alloc(this, GCHandleType.Pinned);
        var thisHandle = GCHandle.ToIntPtr(_thisHandle);

        EditorWindowBridge.SetOnDrawVisible(thisHandle, &OnDrawVisibleCallback);
        EditorWindowBridge.SetOnToolbalDraw(thisHandle, &OnToolbarDrawCallback);
        EditorWindowBridge.SetOnShow(thisHandle, &OnShowCallback);
        EditorWindowBridge.SetOnHide(thisHandle, &OnHideCallback);
    }
}
