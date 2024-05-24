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
    public:
        static const Guid          Null;
        static inline const String NullString = "00000000-0000-0000-0000-000000000000";

    public:
        using uuid::uuid;

        constexpr Guid(
            uint32_t data1   = 0,
            uint16_t data2   = 0,
            uint16_t data3   = 0,
            uint8_t  data4_0 = 0,
            uint8_t  data4_1 = 0,
            uint8_t  data4_2 = 0,
            uint8_t  data4_3 = 0,
            uint8_t  data4_4 = 0,
            uint8_t  data4_5 = 0,
            uint8_t  data4_6 = 0,
            uint8_t  data4_7 = 0)
        {
            data[0]  = static_cast<uint8_t>(data1 >> 24);
            data[1]  = static_cast<uint8_t>(data1 >> 16);
            data[2]  = static_cast<uint8_t>(data1 >> 8);
            data[3]  = static_cast<uint8_t>(data1);
            data[4]  = static_cast<uint8_t>(data2 >> 8);
            data[5]  = static_cast<uint8_t>(data2);
            data[6]  = static_cast<uint8_t>(data3 >> 8);
            data[7]  = static_cast<uint8_t>(data3);
            data[8]  = data4_0;
            data[9]  = data4_1;
            data[10] = data4_2;
            data[11] = data4_3;
            data[12] = data4_4;
            data[13] = data4_5;
            data[14] = data4_6;
            data[15] = data4_7;
        }

        constexpr Guid(
            const uuid& other)
        {
            std::copy_n(other.data, std::size(other.data), data);
        }

        constexpr Guid& operator=(
            const uuid& other)
        {
            if (this != &other)
            {
                std::copy_n(other.data, std::size(other.data), data);
            }
            return *this;
        }

        constexpr Guid(
            uuid&& other)
        {
            std::copy_n(other.data, std::size(other.data), data);
        }

        constexpr Guid& operator=(
            uuid&& other)
        {
            if (this != &other)
            {
                std::copy_n(other.data, std::size(other.data), data);
            }
            return *this;
        }

        /// <summary>
        /// Convert guid to string
        /// </summary>
        [[nodiscard]] String ToString() const;

        /// <summary>
        /// Convert guid to wide string
        /// </summary>
        [[nodiscard]] String ToWideString() const;

        /// <summary>
        /// Generate guid from string
        /// </summary>
        [[nodiscard]] static Guid FromString(
            StringView Id);

        /// <summary>
        /// Generate a random guid not null
        /// </summary>
        [[nodiscard]] static Guid Random();

    private:
        friend class boost::serialization::access;
        template<typename ArchiveTy>
        void serialize(
            ArchiveTy& archive,
            uint32_t)
        {
            archive & data;
        }
    };
} // namespace Ame

namespace std
{
    template<>
    struct hash<Ame::Guid>
    {
        size_t operator()(
            const Ame::Guid& value) const noexcept;
    };
} // namespace std
