#pragma once

#include <DiligentCore/Common/interface/RefCntAutoPtr.hpp>
#include <Core/String.hpp>

namespace Dg = Diligent;

namespace Ame
{
    using UId          = Dg::INTERFACE_ID;
    using INTERFACE_ID = Dg::INTERFACE_ID;

    struct AME_ENGINE_API UIdUtils
    {
        struct Hasher
        {
            size_t operator()(const UId& id) const;
        };

        struct Comparer
        {
            bool operator()(const UId& lhs, const UId& rhs) const;
        };

        [[nodiscard]] static bool IsNull(const UId& id);

        [[nodiscard]] static UId    FromString(const String& str);
        [[nodiscard]] static String ToString(const UId& id);

        [[nodiscard]] static UId           Generate();
        [[nodiscard]] static const UId&    Null();
        [[nodiscard]] static const String& NullString();
    };
} // namespace Ame
