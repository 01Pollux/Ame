#pragma once

#include <Core/Interface.hpp>
#include <cereal/archives/portable_binary.hpp>

namespace Ame
{
    using BinaryIArchiver = cereal::PortableBinaryInputArchive;
    using BinaryOArchiver = cereal::PortableBinaryOutputArchive;

    class ISerializable : public IObjectWithCallback
    {
    public:
        using IObjectWithCallback::IObjectWithCallback;

        virtual void Serialize(BinaryOArchiver&) const {};
        virtual void Deserialize(BinaryIArchiver&) {};
    };
} // namespace Ame

namespace cereal
{
    //! Serialization for UId, if binary data is supported
    template<class Archive> inline void CEREAL_SAVE_FUNCTION_NAME(Archive& ar, const Ame::UId& uid)
    {
        ar(binary_data(&uid, sizeof(Ame::UId)));
    }

    //! Serialization for Uid, if binary data is supported
    template<class Archive> inline void CEREAL_LOAD_FUNCTION_NAME(Archive& ar, Ame::UId& uid)
    {
        ar(binary_data(&uid, sizeof(Ame::UId)));
    }
} // namespace cereal