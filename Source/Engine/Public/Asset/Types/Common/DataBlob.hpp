#pragma once

#include <Asset/Asset.hpp>
#include <Core/BufferBlob.hpp>

namespace Ame::Asset::Common
{
    class DataBlobAsset : public IAsset
    {
    public:
        IMPLEMENT_QUERY_INTERFACE_IN_PLACE_SUBOJECTS(IID_DataBlobAsset, IAsset, m_Blob);

    private:
        IMPLEMENT_INTERFACE_CTOR(DataBlobAsset) : IAsset(counters)
        {
        }

    public:
        void Serialize(BinaryOArchiver& ar) const override
        {
            IAsset::Serialize(ar);

            ar(m_Blob->GetSize());
            ar(cereal::binary_data(std::bit_cast<const std::byte*>(m_Blob->GetConstDataPtr()), m_Blob->GetSize()));
        }

        void Deserialize(BinaryIArchiver& ar) override
        {
            IAsset::Deserialize(ar);

            size_t size;
            ar(size);
            m_Blob->Resize(size);
            ar(cereal::binary_data(std::bit_cast<std::byte*>(m_Blob->GetDataPtr()), m_Blob->GetSize()));
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
        void Set(Ptr<IDataBlob> blob)
        {
            m_Blob = std::move(blob);
        }

    private:
        Ptr<IDataBlob> m_Blob;
    };
} // namespace Ame::Asset::Common