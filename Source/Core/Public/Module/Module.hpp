#pragma once

#include <span>
#include <vector>
#include <Core/Interface.hpp>

namespace Ame
{
    // {5F13AC67-C708-4999-BDCB-56301717EBAB}
    static const UId IID_BaseModule = { 0x5f13ac67, 0xc708, 0x4999, { 0xbd, 0xcb, 0x56, 0x30, 0x17, 0x17, 0xeb, 0xab } };

    class IModule : public IObject
    {
    public:
        IModule(
            const UId& id) :
            m_Id(id)
        {
        }

        IModule(
            const UId&                 id,
            std::initializer_list<UId> dependencies) :
            m_Id(id),
            m_Dependencies(std::move(dependencies))
        {
        }

        /// <summary>
        /// Initialize the module
        /// </summary>
        virtual void Initialize()
        {
        }

        /// <summary>
        /// Shutdown the module
        /// </summary>
        virtual void Shutdown()
        {
        }

        /// <summary>
        /// Get the module's id
        /// </summary>
        [[nodiscard]] const UId& GetId() const
        {
            return m_Id;
        }

        /// <summary>
        /// Get the module's dependencies
        /// </summary>
        [[nodiscard]] std::span<const UId> GetDependencies() const
        {
            return m_Dependencies;
        }

    private:
        UId              m_Id;
        std::vector<UId> m_Dependencies;
    };
} // namespace Ame