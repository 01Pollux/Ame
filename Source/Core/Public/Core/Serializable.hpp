#pragma once

#include <Core/Interface.hpp>
#include <cereal/archives/portable_binary.hpp>

namespace Ame
{
    // {A219B4F5-332F-4E22-8940-ECA447959315}
    static constexpr UId IID_BaseSerializable = { 0xa219b4f5, 0x332f, 0x4e22, { 0x89, 0x40, 0xec, 0xa4, 0x47, 0x95, 0x93, 0x15 } };

    using BinaryIArchiver = cereal::PortableBinaryInputArchive;
    using BinaryOArchiver = cereal::PortableBinaryOutputArchive;

    class ISerializable : public IObject
    {
    public:
        virtual void AME_METHOD(Serialize)(BinaryOArchiver& ar) const = 0;
        virtual void AME_METHOD(Deserialize)(BinaryIArchiver& ar)     = 0;
    };
} // namespace Ame
