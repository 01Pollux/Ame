using System.Runtime.InteropServices;

namespace AmeSharp.RuntimeHost.Runtime;

public class GCBridge
{
    [UnmanagedCallersOnly]
    public static void CollectAll()
    {
        GC.Collect();
    }

    [UnmanagedCallersOnly]
    public static void Collect(int generation, char mode, bool blocking, bool compacting)
    {
        GC.Collect(generation, (GCCollectionMode)mode, blocking, compacting);
    }

    [UnmanagedCallersOnly]
    public static void WaitForPendingFinalizers()
    {
        GC.WaitForPendingFinalizers();
    }
}
