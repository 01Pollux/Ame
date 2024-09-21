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

    public IEditorWindow(IntPtr obj) : base(obj, true) { }
    public IEditorWindow(string path) : base(EditorWindowBridge.Create(path), false) => InitialzeCallbacks();

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
