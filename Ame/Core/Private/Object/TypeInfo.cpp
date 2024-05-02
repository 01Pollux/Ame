#include <Object/TypeInfo.hpp>
#include <Core/String.hpp>

namespace Ame
{
    TypeInfo::TypeInfo(
        StringView      Name,
        const TypeInfo* Parent) :
        m_Hash(Strings::Hash(Name)),
        m_Name(Name.data(), Name.size()),
        m_Parent(Parent)
    {
    }

    bool TypeInfo::IsTypeOf(
        StringView Name) const
    {
        return IsTypeOf(Strings::Hash(Name));
    }

    bool TypeInfo::IsChildOf(
        StringView Name) const
    {
        return IsChildOf(Strings::Hash(Name));
    }
} // namespace Ame