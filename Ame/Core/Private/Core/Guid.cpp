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
    const Guid Guid::c_Null{};

    String Guid::ToString() const
    {
        String str(36, '\0');
        boost::uuids::to_chars(*this, str.data());
        return str;
    }

    WideString Guid::ToWideString() const
    {
        WideString str(36, L'\0');
        boost::uuids::to_chars(*this, str.data());
        return str;
    }

    Guid Guid::FromString(
        StringView id)
    {
        return Guid{ boost::uuids::string_generator()(id.begin(), id.end()) };
    }

    Guid Guid::Random()
    {
        Guid id;

        boost::uuids::random_generator generator;
        while (true)
        {
            generator().swap(id);
            if (id != c_Null)
            {
                return id;
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
