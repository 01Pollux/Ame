#pragma once

#include <span>
#include <vector>
#include <Core/Interface.hpp>

namespace Ame
{
    // {10120611-BB6E-4DBE-BF08-AED5E37B7431}
    static constexpr UId IID_BaseSubmodule = { 0x10120611, 0xbb6e, 0x4dbe, { 0xbf, 0x8, 0xae, 0xd5, 0xe3, 0x7b, 0x74, 0x31 } };

    class ISubmodule : public IObject
    {
    public:
        ISubmodule(
            const UId& id) :
            m_Id(id)
        {
        }

        virtual ~ISubmodule() = default;

        /// <summary>
        /// Get the submodule's id
        /// </summary>
        [[nodiscard]] const UId& GetId() const
        {
            return m_Id;
        }

    private:
        UId m_Id;
    };
} // namespace Ame