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
        InterfaceContext(IPlugin* owner, IObject* object) : m_Plugin(owner), m_Object(object)
        {
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
        
        void RemoveDependencies(IPlugin* plugin)
        {
            m_Dependencies.erase(plugin);
        }

        void DropDependencies()
        {
            for (auto& plugin : m_Dependencies)
            {
                plugin->OnInterfaceDrop(m_Object);
            }
            m_Dependencies.clear();
        }

        void DropInterface()
        {
            DropDependencies();
            m_Plugin = nullptr;
            m_Object = {};
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