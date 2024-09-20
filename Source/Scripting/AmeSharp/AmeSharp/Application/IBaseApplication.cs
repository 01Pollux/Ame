using AmeSharp.Config;
using AmeSharp.Core.Log;
using AmeSharp.Engine;

namespace AmeSharp.Application;

public abstract class IBaseApplication
{
    public IAmeEngine Engine { get; private set; }

    public IBaseApplication(ApplicationConfig config)
    {
        Engine = new();
        IntiailizeEngine(config);
    }

    public virtual void OnLoad() { }
    public virtual void OnInitialize() { }
    public virtual void OnShutdown() { }
    public virtual void OnUnload() { }

    public void Run()
    {
        ILogger.Instance?.Trace("Application started");

        OnLoad();
        ILogger.Instance?.Trace("Application initialized");

        OnInitialize();
        ILogger.Instance?.Trace("Application running");

        while (Engine.IsRunning())
        {
            Engine.Tick();
        }

        OnShutdown();
        ILogger.Instance?.Trace("Application stopped");

        OnUnload();
        ILogger.Instance?.Trace("Application unloaded");
    }

    //

    private void IntiailizeEngine(ApplicationConfig config)
    {
        var registry = Engine.Registry;
        config.RegisterInterface(registry);
        Engine.RefreshSubmoduleCache();
    }
}
