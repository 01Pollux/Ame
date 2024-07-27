#pragma once

#include <Asset/Asset.hpp>
#include <Core/BufferBlob.hpp>

namespace Ame::Asset::Common
{
    // {1BB9A4FA-2503-4525-BD6D-23835181896F}
    static constexpr UId IID_DataBlobAsset = { 0x1bb9a4fa, 0x2503, 0x4525, { 0xbd, 0x6d, 0x23, 0x83, 0x51, 0x81, 0x89, 0x6f } };

    class DataBlobAsset : public BaseObject<IAsset>
    {
    public:
        using Base = BaseObject<IAsset>;

        IMPLEMENT_QUERY_INTERFACE_IN_PLACE_SUBOJECTS2(
            IID_DataBlobAsset, IID_BaseSerializable, Base,
            m_Blob);

        DataBlobAsset(
            IReferenceCounters* counters) :
            Base(counters)
        {
        }

    public:
        /// <summary>
        /// Get data of the buffer
        /// </summary>
        [[nodiscard]] IDataBlob* Get() const noexcept
        {
            return m_Blob;
        }

        /// <summary>
        /// Set data of the buffer
        /// </summary>
        void Set(
            Ptr<IDataBlob> blob)
        {
            m_Blob = std::move(blob);
        }

    private:
        Ptr<IDataBlob> m_Blob;
    };
} // namespace Ame::Asset::Common