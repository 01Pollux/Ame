#pragma once

#include <span>
#include <vector>
#include <Core/Interface.hpp>

namespace Ame
{
    // {5F13AC67-C708-4999-BDCB-56301717EBAB}
    inline constexpr UId IID_BaseModule{ 0x5f13ac67, 0xc708, 0x4999, { 0xbd, 0xcb, 0x56, 0x30, 0x17, 0x17, 0xeb, 0xab } };

    class IModule : public IObject
    {
    public:
        IModule(
            const UId& id) :
            m_Id(id)
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

    private:
        UId m_Id;
    };
} // namespace Ame