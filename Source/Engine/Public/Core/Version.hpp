#pragma once

#include <array>
#include <Core/Ame.hpp>
#include <Core/String.hpp>

namespace Ame
{
    /// <summary>
    /// Version class
    /// Contains major, minor, build and revision
    /// </summary>
    struct TVersion
    {
    public:
        enum class Type : unsigned char
        {
            Major,
            Minor,
            Build,
            Revision
        };
        using value_type = unsigned short;

        constexpr TVersion() = default;

        constexpr TVersion(value_type maj, value_type min, value_type build = 0, value_type rev = 0) noexcept :
            m_Major(maj), m_Minor(min), m_Build(build), m_Revision(rev)
        {
        }

        constexpr explicit TVersion(const std::array<value_type, 4>& arr) noexcept :
            m_Major(arr[0]), m_Minor(arr[1]), m_Build(arr[2]), m_Revision(arr[3])
        {
        }

        template<StringType StrTy> constexpr explicit TVersion(const StrTy& str) noexcept : TVersion(FromString(str))
        {
        }

        [[nodiscard]] constexpr value_type operator[](Type type) const noexcept
        {
            switch (type)
            {
            case Type::Major:
                return m_Major;
            case Type::Minor:
                return m_Minor;
            case Type::Build:
                return m_Build;
            case Type::Revision:
            default:
                return m_Revision;
            }
        }

        [[nodiscard]] constexpr value_type& operator[](Type type) noexcept
        {
            switch (type)
            {
            case Type::Major:
                return m_Major;
            case Type::Minor:
                return m_Minor;
            case Type::Build:
                return m_Build;
            case Type::Revision:
            default:
                return m_Revision;
            }
        }

        [[nodiscard]] constexpr value_type& Major() noexcept
        {
            return m_Major;
        }
        [[nodiscard]] constexpr value_type Major() const noexcept
        {
            return m_Major;
        }

        [[nodiscard]] constexpr value_type& Minor() noexcept
        {
            return m_Minor;
        }
        [[nodiscard]] constexpr value_type Minor() const noexcept
        {
            return m_Minor;
        }

        [[nodiscard]] constexpr value_type& Build() noexcept
        {
            return m_Build;
        }
        [[nodiscard]] constexpr value_type Build() const noexcept
        {
            return m_Build;
        }

        [[nodiscard]] constexpr value_type& Revision() noexcept
        {
            return m_Revision;
        }
        [[nodiscard]] constexpr value_type Revision() const noexcept
        {
            return m_Revision;
        }

        /// <summary>
        /// Convert to array
        /// </summary>
        [[nodiscard]] constexpr auto ToArray() const noexcept
        {
            return std::array{ Major(), Minor(), Build(), Revision() };
        }

        /// <summary>
        /// Convert to array
        /// </summary>
        [[nodiscard]] auto ToString() const noexcept
        {
            return std::format("{:d}.{:d}.{:d}.{:d}", Major(), Minor(), Build(), Revision());
        }

        /// <summary>
        /// Convert from string to version
        /// </summary>
        template<StringType StrTy> [[nodiscard]] static constexpr TVersion FromString(const StrTy& str)
        {
            int      iter   = 0;
            Type     curVer = Type::Major;
            TVersion version;

            for (const auto C : str)
            {
                if (curVer > Type::Revision)
                    break;

                if (C == '.')
                {
                    iter   = 0;
                    curVer = Type(int(curVer) + 1);
                    continue;
                }
                else if (C < '0' || C > '9')
                {
                    break;
                }

                iter *= 10;
                iter += C - '0';

                version[curVer] = static_cast<value_type>(iter);
            }

            return version;
        }

        constexpr auto operator<=>(const TVersion&) const noexcept = default;

    private:
        value_type m_Major = 0, m_Minor = 0, m_Build = 0, m_Revision = 0;
    };
} // namespace Ame