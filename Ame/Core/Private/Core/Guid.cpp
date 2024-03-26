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

    StringU8 Guid::ToString() const
    {
        return boost::uuids::to_string(*this);
    }

    String Guid::ToWideString() const
    {
        return boost::uuids::to_wstring(*this);
    }

    Guid Guid::FromString(
        const std::string& Id)
    {
        return Guid{ boost::uuids::string_generator()(Id) };
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
        const Ame::Guid& Value) const noexcept
    {
        return boost::uuids::hash_value(Value);
    };
} // namespace std
