#pragma once

#include <Core/String.hpp>

namespace Ame
{
    class TypeInfo;

    namespace Concepts
    {
        template<typename T>
        concept TypeInfo = requires(T t) {
            {
                t.GetTypeInfo()
            } -> std::convertible_to<const Ame::TypeInfo&>;
        };
    } // namespace Concepts

    class TypeInfo
    {
    public:
        TypeInfo(
            std::string_view Name,
            const TypeInfo*  Parent = nullptr);

        [[nodiscard]] uint64_t GetHash() const
        {
            return m_Hash;
        }
        [[nodiscard]] eastl::string_view GetName() const
        {
            return m_Name.c_str();
        }
        [[nodiscard]] const TypeInfo* GetParent() const
        {
            return m_Parent;
        }

        [[nodiscard]] bool IsTypeOf(
            uint64_t Hash) const
        {
            return m_Hash == Hash;
        }
        [[nodiscard]] bool IsTypeOf(
            std::string_view Name) const;
        [[nodiscard]] bool IsTypeOf(
            const TypeInfo& Type) const
        {
            return IsTypeOf(Type.GetHash());
        }
        template<Concepts::TypeInfo Ty>
        [[nodiscard]] bool IsTypeOf() const
        {
            return IsTypeOf(Ty::GetTypeInfo().GetHash());
        }

        [[nodiscard]] bool IsChildOf(
            uint64_t Hash) const
        {
            auto This = this;
            while (This)
            {
                if (This->m_Hash == Hash)
                    return true;
                This = This->m_Parent;
            }
            return false;
        }
        [[nodiscard]] bool IsChildOf(
            std::string_view Name) const;
        [[nodiscard]] bool IsChildOf(
            const TypeInfo& Type) const
        {
            return IsChildOf(Type.GetHash());
        }

    private:
        uint64_t        m_Hash;
        eastl::string   m_Name;
        const TypeInfo* m_Parent;
    };
} // namespace Ame