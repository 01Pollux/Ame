using AmeEditorCore.Bridge.EditorWindow;
using AmeSharp.Core.Base;
using AmeSharp.Core.Utils.Callbacks;
using System.Runtime.InteropServices;

namespace AmeEditorCore.EditorWindow;

public class IEditorWindow : IBaseObject
{
    [UnmanagedFunctionPointer(CallingConvention.Cdecl)]
    private delegate void EditorWindowCallbacks();

    private readonly CallbackHandler _onDrawVisible;
    private readonly CallbackHandler _onToolbarDraw;
    private readonly CallbackHandler _onShow;
    private readonly CallbackHandler _onHide;

    public IEditorWindow(string path) : base(EditorWindowBridge.Create(path))
    {
        unsafe
        {
            _onDrawVisible = new(OnDrawVisible, (callbackImpl, thisHandle) => { EditorWindowBridge.SetOnDrawVisible(NativePointer, callbackImpl); return NativePointer; });
            _onToolbarDraw = new(OnToolbarDraw, (callbackImpl, thisHandle) => { EditorWindowBridge.SetOnToolbalDraw(NativePointer, callbackImpl); return NativePointer; });
            _onShow = new(OnShow, (callbackImpl, thisHandle) => { EditorWindowBridge.SetOnShow(NativePointer, callbackImpl); return NativePointer; });
            _onHide = new(OnHide, (callbackImpl, thisHandle) => { EditorWindowBridge.SetOnHide(NativePointer, callbackImpl); return NativePointer; });
        }
    }

    public string FullPath => EditorWindowBridge.GetFullPath(NativePointer).ToString();

    public virtual void OnDrawVisible() { }
    public virtual void OnToolbarDraw() { }
    public virtual void OnShow() { }
    public virtual void OnHide() { }
}
