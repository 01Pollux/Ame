using AmeSharp.Bridge.Core.Base;

namespace AmeSharp.Core.Base;

// a tag to indicate that the object was created inside c# code
public abstract class BaseObject() : IBaseObject(BaseObjectBridge.Create(), true, true)
{
}
