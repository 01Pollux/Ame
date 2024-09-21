using AmeSharp.Bridge.Core.Internal;
using System.Runtime.InteropServices;

namespace AmeSharp.Core.Internal
{
    public class IAbstractStorage : IDisposable
    {
        private IAbstractStorage() { }
        ~IAbstractStorage() => DisposeNoGcSupress();

        private static readonly Lazy<IAbstractStorage> _instance = new(() => new IAbstractStorage());
        private IntPtr _storage = AbstractStorageBridge.Create();

        public static IAbstractStorage Instance => _instance.Value;

        //

        public static void Set<T>(IntPtr key, T value) where T : class
        {
            var obj = GCHandle.ToIntPtr(GCHandle.Alloc(value, GCHandleType.Normal));
            AbstractStorageBridge.Set(Instance._storage, key, obj);
        }

        public static T? Get<T>(IntPtr key) where T : class
        {
            var obj = AbstractStorageBridge.Get(Instance._storage, key);
            return obj == IntPtr.Zero ? null : GCHandle.FromIntPtr(obj).Target as T;
        }

        public static void Remove(IntPtr key)
        {
            AbstractStorageBridge.Remove(Instance._storage, key);
        }

        //

        public void Dispose()
        {
            DisposeNoGcSupress();
            GC.SuppressFinalize(this);
        }

        protected void DisposeNoGcSupress()
        {
            if (_storage != IntPtr.Zero)
            {
                AbstractStorageBridge.Release(_storage);
                _storage = IntPtr.Zero;
            }
        }
    }
}
