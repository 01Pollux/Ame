using AmeEditor.Config;
using AmeSharp.Core.Application;

namespace AmeEditor.Application;

internal class EditorApplication(EditorConfig config) : IBaseApplication(config.Application)
{
}
