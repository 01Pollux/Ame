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

        EditorWindowBridge.SetOnDrawVisible(NativePointer, &OnDrawVisibleCallback);
        EditorWindowBridge.SetOnToolbalDraw(NativePointer, &OnToolbarDrawCallback);
        EditorWindowBridge.SetOnShow(NativePointer, &OnShowCallback);
        EditorWindowBridge.SetOnHide(NativePointer, &OnHideCallback);
    }
}

[Guid("00000000-0000-0000-0000-0000000000A0")]
public class XXXIEditorWindow : IEditorWindow
{
    public XXXIEditorWindow() : base("XXX") { }

    public override void OnDrawVisible() { Console.WriteLine("overwritte"); }
    public override void OnToolbarDraw() { Console.WriteLine("overwritte"); }
    public override void OnShow() { Console.WriteLine("overwritte"); }
    public override void OnHide() { Console.WriteLine("overwritte"); }
}
