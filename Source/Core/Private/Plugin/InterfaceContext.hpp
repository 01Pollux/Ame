#pragma once

#include <filesystem>
#include <set>
#include <boost/dll.hpp>
#include <Plugin/Plugin.hpp>

namespace Ame
{
    class InterfaceContext
    {
    public:
        InterfaceContext(IPlugin* owner, IObject* object) :
            m_Plugin(owner),
            m_Object(object)
        {
        }

        ~InterfaceContext()
        {
            for (auto& plugin : m_Dependencies)
            {
                plugin->OnInterfaceDrop(m_Object);
            }
            if (m_Plugin)
            {
                m_Plugin->OnInterfaceDrop(m_Object);
            }
        }

    public:
        [[nodiscard]] IPlugin* GetPlugin() const noexcept
        {
            return m_Plugin;
        }

        [[nodiscard]] IObject* GetObject() const noexcept
        {
            return m_Object;
        }

        void AddDependencies(IPlugin* plugin)
        {
            m_Dependencies.insert(plugin);
        }

        [[nodiscard]] bool HasDependencies() const noexcept
        {
            return !m_Dependencies.empty();
        }

        [[nodiscard]] auto& GetDependencies() const noexcept
        {
            return m_Dependencies;
        }

    private:
        IPlugin*           m_Plugin;
        Ptr<IObject>       m_Object;
        std::set<IPlugin*> m_Dependencies;
    };
} // namespace Ame