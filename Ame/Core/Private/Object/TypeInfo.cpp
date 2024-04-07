#include <Object/TypeInfo.hpp>
#include <Core/String.hpp>

namespace Ame
{
    TypeInfo::TypeInfo(
        std::string_view Name,
        const TypeInfo*  Parent) :
        m_Hash(Strings::Hash(Name)),
        m_Name(Name.data(), Name.size()),
        m_Parent(Parent)
    {
    }

    bool TypeInfo::IsTypeOf(
        std::string_view Name) const
    {
        return IsTypeOf(Strings::Hash(Name));
    }

    bool TypeInfo::IsChildOf(
        std::string_view Name) const
    {
        return IsChildOf(Strings::Hash(Name));
    }
} // namespace Ame