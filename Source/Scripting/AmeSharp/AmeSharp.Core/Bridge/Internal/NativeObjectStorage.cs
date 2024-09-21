using AmeSharp.Bridge.Core.Internal;
using AmeSharp.Core.Base;
using AmeSharp.Core.Log;
using System.Runtime.InteropServices;

namespace AmeSharp.Core.Bridge.Internal
{
    internal class NativeObjectStorage : IDisposable
    {
        private static readonly Lazy<NativeObjectStorage> _instance = new(() => new NativeObjectStorage());
        private nint _storage = AbstractStorageBridge.Create();

        private NativeObjectStorage() { }
        ~NativeObjectStorage() => DisposeNoGcSupress();

        //

        public static INativeObject? Get(nint nativePointer)
        {
            var handle = AbstractStorageBridge.Get(_instance.Value._storage, nativePointer);
            return handle == nint.Zero ? null : GCHandle.FromIntPtr(handle).Target as INativeObject;
        }

        /// <summary>
        /// Get the object from the storage, if it doesn't exist, create a new instance.
        /// This will create an instance only if the object is not in the storage (was created externally).
        /// </summary>
        public static T? Get<T>(nint nativePointer) where T : INativeObject
        {
            var handle = AbstractStorageBridge.Get(_instance.Value._storage, nativePointer);
            if (handle == nint.Zero)
            {
                var type = typeof(T);
                // check if the object has a constructor with a single IntPtr parameter
                if (type.GetConstructor([typeof(nint), typeof(bool)]) is not null)
                {
                    try
                    {
                        var instance = Activator.CreateInstance(type, nativePointer, true) as T;
                        if (instance is not null)
                        {
                            Set(nativePointer, instance);
                        }
                        return instance;
                    }
                    catch (Exception ex)
                    {
                        ILogger.Instance?.Error(ex.Message);
                    }
                }
                return null;
            }
            else
            {
                return GCHandle.FromIntPtr(handle).Target as T;
            }
        }

        public static void Set(nint nativePointer, INativeObject value)
        {
            var handle = GCHandle.ToIntPtr(GCHandle.Alloc(value, GCHandleType.Weak));
            AbstractStorageBridge.Set(_instance.Value._storage, nativePointer, handle);
        }

        public static void Remove(nint nativePointer)
        {
            var handle = AbstractStorageBridge.Get(_instance.Value._storage, nativePointer);
            if (handle != nint.Zero)
            {
                GCHandle.FromIntPtr(handle).Free();
                AbstractStorageBridge.Remove(_instance.Value._storage, nativePointer);
            }
        }

        //

        public void Dispose()
        {
            DisposeNoGcSupress();
            GC.SuppressFinalize(this);
        }

        protected void DisposeNoGcSupress()
        {
            if (_storage != nint.Zero)
            {
                AbstractStorageBridge.Release(_storage);
                _storage = nint.Zero;
            }
        }
    }
}
