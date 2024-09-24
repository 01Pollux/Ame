using AmeEditorCore.Bridge.EditorWindow;
using AmeSharp.Core.Base;
using System.Runtime.InteropServices;

namespace AmeEditorCore.EditorWindow;

[Guid("97FB68DA-A89C-4938-A158-9402F5534427")]
public abstract class IEditorWindow : IBaseObject
{
    [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
    internal delegate void EditorWindowCallbacks(IntPtr _thisPointer);

    public IEditorWindow(string path) : base(EditorWindowBridge.Create(path), true, false)
    {
        InitialzeCallbacks();
    }

    public string FullPath => EditorWindowBridge.GetFullPath(Handle);

    public virtual void OnDrawVisible() { }
    public virtual void OnToolbarDraw() { }
    public virtual void OnShow() { }
    public virtual void OnHide() { }

    //

    private void InitialzeCallbacks()
    {
        EditorWindowBridge.SetOnDrawVisible(Handle, (_) => OnDrawVisible());
        EditorWindowBridge.SetOnToolbalDraw(Handle, (_) => OnToolbarDraw());
        EditorWindowBridge.SetOnShow(Handle, (_) => OnShow());
        EditorWindowBridge.SetOnHide(Handle, (_) => OnHide());
    }
}
