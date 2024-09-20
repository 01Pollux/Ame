using AmeEditor.Config;
using AmeSharp.Application;

namespace AmeEditor.Application;

internal class EditorApplication(EditorConfig config) : IBaseApplication(config.Application)
{
    public override void OnLoad()
    {
    }
}
