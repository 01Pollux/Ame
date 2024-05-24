#include <Core/Guid.hpp>

#include <boost/uuid/uuid_io.hpp>
#include <boost/uuid/string_generator.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid_hash.hpp>
#include <boost/uuid/nil_generator.hpp>

#ifdef AME_PLATFORM_WINDOWS
#pragma comment(lib, "bcrypt.lib")
#endif

namespace Ame
{
    const Guid Guid::Null{};

    String Guid::ToString() const
    {
        String Str(36, '\0');
        boost::uuids::to_chars(*this, Str.data());
        return Str;
    }

    String Guid::ToWideString() const
    {
        String Str(36, L'\0');
        boost::uuids::to_chars(*this, Str.data());
        return Str;
    }

    Guid Guid::FromString(
        StringView Id)
    {
        return Guid{ boost::uuids::string_generator()(Id.begin(), Id.end()) };
    }

    Guid Guid::Random()
    {
        Guid Id;

        boost::uuids::random_generator Generator;
        while (true)
        {
            Generator().swap(Id);
            if (Id != Null)
            {
                return Id;
            }
        }
        std::unreachable();
    }
} // namespace Ame

namespace std
{
    size_t hash<Ame::Guid>::operator()(
        const Ame::Guid& value) const noexcept
    {
        return boost::uuids::hash_value(value);
    };
} // namespace std
