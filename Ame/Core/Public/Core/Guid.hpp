#pragma once

#include <boost/uuid/uuid.hpp>
#include <Core/String.hpp>

namespace boost::serialization
{
    class access;
} // namespace boost::serialization

namespace Ame
{
    struct Guid : boost::uuids::uuid
    {
    public:
        static const Guid   c_Null;
        static const String c_NullString;

    public:
        using uuid::uuid;

        constexpr Guid(
            uint32_t data1  = 0,
            uint16_t data2  = 0,
            uint16_t data3  = 0,
            uint8_t  data40 = 0,
            uint8_t  data41 = 0,
            uint8_t  data42 = 0,
            uint8_t  data43 = 0,
            uint8_t  data44 = 0,
            uint8_t  data45 = 0,
            uint8_t  data46 = 0,
            uint8_t  data47 = 0)
        {
            data[0]  = static_cast<uint8_t>(data1 >> 24);
            data[1]  = static_cast<uint8_t>(data1 >> 16);
            data[2]  = static_cast<uint8_t>(data1 >> 8);
            data[3]  = static_cast<uint8_t>(data1);
            data[4]  = static_cast<uint8_t>(data2 >> 8);
            data[5]  = static_cast<uint8_t>(data2);
            data[6]  = static_cast<uint8_t>(data3 >> 8);
            data[7]  = static_cast<uint8_t>(data3);
            data[8]  = data40;
            data[9]  = data41;
            data[10] = data42;
            data[11] = data43;
            data[12] = data44;
            data[13] = data45;
            data[14] = data46;
            data[15] = data47;
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
        [[nodiscard]] WideString ToWideString() const;

        /// <summary>
        /// Generate guid from string
        /// </summary>
        [[nodiscard]] static Guid FromString(
            StringView id);

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
