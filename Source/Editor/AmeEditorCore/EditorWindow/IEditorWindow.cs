using AmeEditorCore.Bridge.EditorWindow;
using AmeSharp.Core.Base;
using System.Runtime.InteropServices;

namespace AmeEditorCore.EditorWindow;

public class IEditorWindow : IBaseObject
{
    [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
    private delegate void EditorWindowCallbacks();

    private readonly EditorWindowCallbacks _onDrawVisible;
    private readonly EditorWindowCallbacks _onToolbarDraw;
    private readonly EditorWindowCallbacks _onShow;
    private readonly EditorWindowCallbacks _onHide;

    public IEditorWindow(string path) : base(EditorWindowBridge.Create(path))
    {
        _onDrawVisible = new(OnDrawVisible);
        _onToolbarDraw = new(OnToolbarDraw);
        _onShow = new(OnShow);
        _onHide = new(OnHide);

        unsafe
        {
            EditorWindowBridge.SetOnDrawVisible(NativePointer, (delegate* unmanaged[Cdecl]<void>)Marshal.GetFunctionPointerForDelegate(_onDrawVisible));
            EditorWindowBridge.SetOnToolbalDraw(NativePointer, (delegate* unmanaged[Cdecl]<void>)Marshal.GetFunctionPointerForDelegate(_onToolbarDraw));
            EditorWindowBridge.SetOnShow(NativePointer, (delegate* unmanaged[Cdecl]<void>)Marshal.GetFunctionPointerForDelegate(_onShow));
            EditorWindowBridge.SetOnHide(NativePointer, (delegate* unmanaged[Cdecl]<void>)Marshal.GetFunctionPointerForDelegate(_onHide));
        }
    }

    public string FullPath => EditorWindowBridge.GetFullPath(NativePointer).ToString();

    public virtual void OnDrawVisible() { }
    public virtual void OnToolbarDraw() { }
    public virtual void OnShow() { }
    public virtual void OnHide() { }
}
