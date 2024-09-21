#pragma once

#include <EcsComponent/Core.hpp>

namespace Ame::Scripting
{
    class IInstance;
} // namespace Ame::Scripting

namespace Ame::Ecs
{
    // {8A752246-8374-4513-94A5-B49382BE32D0}
    inline constexpr UId IID_ScriptInstance{
        0x8a752246, 0x8374, 0x4513, { 0x94, 0xa5, 0xb4, 0x93, 0x82, 0xbe, 0x32, 0xd0 }
    };

    class AME_ENGINE_API ScriptInstance : public ISerializable
    {
    public:
        IMPLEMENT_QUERY_INTERFACE_IN_PLACE(IID_ScriptInstance, ISerializable);

    private:
        IMPLEMENT_INTERFACE_CTOR(ScriptInstance, Scripting::IInstance* instance) :
            ISerializable(counters), m_Instance(instance)
        {
        }

    public:
        /// <summary>
        /// Get the script instance
        /// </summary>
        [[nodiscard]] Scripting::IInstance* GetInstance() const
        {
            return m_Instance;
        }

    private:
        Ptr<Scripting::IInstance> m_Instance;
    };

    //

    AME_ECS_WRAP_COMPONENT_PTR(ScriptInstance);
} // namespace Ame::Ecs