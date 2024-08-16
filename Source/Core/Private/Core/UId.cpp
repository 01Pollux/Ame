#include <Core/UId.hpp>
#include <Core/Hash.hpp>

#include <boost/uuid/random_generator.hpp>

namespace Ame
{
    consteval size_t UIdData4Count()
    {
        return std::size(UId().Data4);
    }

    static_assert(sizeof(UId) == 16, "UId size is not 16 bytes");
    static_assert(UIdData4Count() == 8, "UId::Data4 size is not 8 bytes");

    static const UId    c_Null{};
    static const String c_NullString{ "00000000-0000-0000-0000-000000000000" };

    //

    size_t UIdUtils::Hasher::operator()(
        const UId& id) const
    {
        size_t hash = 0;
        HashCombine(hash, id.Data1);
        HashCombine(hash, id.Data2);
        HashCombine(hash, id.Data3);
        HashCombine(hash, std::begin(id.Data4), std::end(id.Data4));
        return hash;
    }

    bool UIdUtils::Comparer::operator()(
        const UId& lhs, const UId& rhs) const
    {
        return memcmp(&lhs, &rhs, sizeof(UId)) == 0;
    }

    bool UIdUtils::IsNull(const UId& id)
    {
        return memcmp(&id, &c_Null, sizeof(UId)) == 0;
    }

    UId UIdUtils::FromString(const String& str)
    {
        // string must be (8-4-4-4-12) characters long
        static constexpr size_t c_UIdStringLength = 8 + 1 + 4 + 1 + 4 + 1 + 2 * 2 + 1 + 2 * 6;

        UId  uid;
        bool error = false;
        if (str.size() == c_UIdStringLength)
        {
            error = std::from_chars(str.data(), str.data() + 8, uid.Data1, 16).ec != std::errc();
            error |= std::from_chars(str.data() + 9, str.data() + 13, uid.Data2, 16).ec != std::errc();
            error |= std::from_chars(str.data() + 14, str.data() + 18, uid.Data3, 16).ec != std::errc();
            for (size_t i = 0; i < UIdData4Count(); ++i)
            {
                error |= std::from_chars(str.data() + 19 + i * 2, str.data() + 21 + i * 2, uid.Data4[i], 16).ec != std::errc();
            }
        }
        else
        {
            error = true;
        }

        if (error)
        {
            uid = Null();
        }
        return uid;
    }

    String UIdUtils::ToString(const UId& id)
    {
        return std::format(
            "{:08x}-{:04x}-{:04x}-{:02x}{:02x}-{:02x}{:02x}{:02x}{:02x}{:02x}{:02x}",
            id.Data1, id.Data2, id.Data3,
            id.Data4[0], id.Data4[1], id.Data4[2], id.Data4[3],
            id.Data4[4], id.Data4[5], id.Data4[6], id.Data4[7]);
    }

    UId UIdUtils::Generate()
    {
        UId uid;

        boost::uuids::random_generator generator;
        static_assert(sizeof(uid) == sizeof(generator()), "UId and boost::uuids::random_generator have different sizes");
        while (true)
        {
            auto rnd    = generator();
            bool isZero = std::memcmp(&rnd, &c_Null, sizeof(uid)) == 0;
            if (!isZero)
            {
                std::memcpy(&uid, &rnd, sizeof(uid));
                break;
            }
        }

        return uid;
    }

    const UId& UIdUtils::Null()
    {
        return c_Null;
    }

    const String& UIdUtils::NullString()
    {
        return c_NullString;
    }
} // namespace Ame