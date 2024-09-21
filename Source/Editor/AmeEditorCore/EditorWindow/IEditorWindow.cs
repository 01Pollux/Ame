using AmeEditorCore.Bridge.EditorWindow;
using AmeSharp.Core.Base;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;

namespace AmeEditorCore.EditorWindow;

[Guid("97FB68DA-A89C-4938-A158-9402F5534427")]
public class IEditorWindow : IBaseObject
{
    [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
    private delegate void EditorWindowCallbacks();

    public IEditorWindow(IntPtr obj) : base(obj) { }
    public IEditorWindow(string path) : base(EditorWindowBridge.Create(path)) => InitialzeCallbacks();

    public string FullPath => EditorWindowBridge.GetFullPath(NativePointer).ToString();

    public virtual void OnDrawVisible() { }
    public virtual void OnToolbarDraw() { }
    public virtual void OnShow() { }
    public virtual void OnHide() { }

    //

    [UnmanagedCallersOnly(CallConvs = [typeof(CallConvCdecl)])]
    public static void OnDrawVisibleCallback(IntPtr _thisPointer)
    {
        var @this = Get<IEditorWindow>(_thisPointer)!;
        @this.OnDrawVisible();
    }
    [UnmanagedCallersOnly(CallConvs = [typeof(CallConvCdecl)])]
    public static void OnToolbarDrawCallback(IntPtr _thisPointer)
    {
        var @this = Get<IEditorWindow>(_thisPointer)!;
        @this.OnToolbarDraw();
    }
    [UnmanagedCallersOnly(CallConvs = [typeof(CallConvCdecl)])]
    public static void OnShowCallback(IntPtr _thisPointer)
    {
        var @this = Get<IEditorWindow>(_thisPointer)!;
        @this.OnShow();
    }
    [UnmanagedCallersOnly(CallConvs = [typeof(CallConvCdecl)])]
    public static void OnHideCallback(IntPtr _thisPointer)
    {
        var @this = Get<IEditorWindow>(_thisPointer)!;
        @this.OnHide();
    }

    private unsafe void InitialzeCallbacks()
    {
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
