#pragma once

#include <Core/String.hpp>
#include <boost/uuid/uuid.hpp>

namespace boost::serialization
{
    class access;
}

namespace Ame
{
    struct Guid : boost::uuids::uuid
    {
        using uuid::uuid;

        constexpr Guid(
            uint32_t Data1   = 0,
            uint16_t Data2   = 0,
            uint16_t Data3   = 0,
            uint8_t  Data4_0 = 0,
            uint8_t  Data4_1 = 0,
            uint8_t  Data4_2 = 0,
            uint8_t  Data4_3 = 0,
            uint8_t  Data4_4 = 0,
            uint8_t  Data4_5 = 0,
            uint8_t  Data4_6 = 0,
            uint8_t  Data4_7 = 0)
        {
            data[0]  = static_cast<uint8_t>(Data1 >> 24);
            data[1]  = static_cast<uint8_t>(Data1 >> 16);
            data[2]  = static_cast<uint8_t>(Data1 >> 8);
            data[3]  = static_cast<uint8_t>(Data1);
            data[4]  = static_cast<uint8_t>(Data2 >> 8);
            data[5]  = static_cast<uint8_t>(Data2);
            data[6]  = static_cast<uint8_t>(Data3 >> 8);
            data[7]  = static_cast<uint8_t>(Data3);
            data[8]  = Data4_0;
            data[9]  = Data4_1;
            data[10] = Data4_2;
            data[11] = Data4_3;
            data[12] = Data4_4;
            data[13] = Data4_5;
            data[14] = Data4_6;
            data[15] = Data4_7;
        }

        constexpr Guid(const uuid& Other)
        {
            std::copy_n(Other.data, std::size(Other.data), data);
        }

        constexpr Guid& operator=(const uuid& Other)
        {
            if (this != &Other)
            {
                std::copy_n(Other.data, std::size(Other.data), data);
            }
            return *this;
        }

        constexpr Guid(uuid&& Other)
        {
            std::copy_n(Other.data, std::size(Other.data), data);
        }

        constexpr Guid& operator=(uuid&& Other)
        {
            if (this != &Other)
            {
                std::copy_n(Other.data, std::size(Other.data), data);
            }
            return *this;
        }

        static const Guid            Null;
        static inline const StringU8 NullString = "00000000-0000-0000-0000-000000000000";

        /// <summary>
        /// Convert guid to string
        /// </summary>
        [[nodiscard]] StringU8 ToString() const;

        /// <summary>
        /// Convert guid to wide string
        /// </summary>
        [[nodiscard]] String ToWideString() const;

        /// <summary>
        /// Generate guid from string
        /// </summary>
        [[nodiscard]] static Guid FromString(
            const StringU8& Id);

        /// <summary>
        /// Generate a random guid not null
        /// </summary>
        [[nodiscard]] static Guid Random();

    private:
        friend class boost::serialization::access;
        template<typename ArchiveTy>
        void serialize(
            ArchiveTy& Archive,
            uint32_t)
        {
            Archive & data;
        }
    };
} // namespace Ame

namespace std
{
    template<>
    struct hash<Ame::Guid>
    {
        size_t operator()(
            const Ame::Guid& Value) const noexcept;
    };
} // namespace std
