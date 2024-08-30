#pragma once

#include <vector>
#include <Core/Interface.hpp>
#include <DiligentCore/Primitives/interface/DataBlob.h>

namespace Ame
{
    using IDataBlob = Dg::IDataBlob;

    // {F578FF0D-ABD2-4514-9D32-7CB454D4A73B}
    inline constexpr UId IID_DataBlob{ 0xf578ff0d, 0xabd2, 0x4514, { 0x9d, 0x32, 0x7c, 0xb4, 0x54, 0xd4, 0xa7, 0x3b } };

    class RawDataBlob : public BaseObject<IDataBlob>
    {
    public:
        using Base = BaseObject<IDataBlob>;

        IMPLEMENT_QUERY_INTERFACE_IN_PLACE(
            IID_DataBlob, Base);

        RawDataBlob(
            IReferenceCounters* counters,
            const std::byte*    data        = nullptr,
            size_t              initialSize = 0) :
            Base(counters)
        {
            m_Data.resize(initialSize);
            if (data != nullptr)
            {
                std::copy(data, data + initialSize, m_Data.begin());
            }
        }

        RawDataBlob(
            IReferenceCounters* counters,
            const IDataBlob*    otherBlob) :
            Base(counters)
        {
            m_Data.insert(
                m_Data.end(),
                std::bit_cast<const std::byte*>(otherBlob->GetConstDataPtr()),
                std::bit_cast<const std::byte*>(otherBlob->GetConstDataPtr()) + otherBlob->GetSize());
        }

    public:
        void Resize(size_t newSize) override
        {
            m_Data.resize(newSize);
        }

        [[nodiscard]] size_t GetSize() const override
        {
            return m_Data.size();
        }

        [[nodiscard]] void* GetDataPtr() override
        {
            return m_Data.data();
        }

        [[nodiscard]] const void* GetConstDataPtr() const override
        {
            return m_Data.data();
        }

        template<typename Ty>
        [[nodiscard]] Ty* GetDataPtr()
        {
            return std::bit_cast<Ty*>(GetDataPtr());
        }

        template<typename Ty>
        [[nodiscard]] const Ty* GetConstDataPtr() const
        {
            return std::launder(std::bit_cast<const Ty*>(GetConstDataPtr()));
        }

        [[nodiscard]] auto& Get() noexcept
        {
            return m_Data;
        }

        [[nodiscard]] const auto& Get() const noexcept
        {
            return m_Data;
        }

    private:
        std::vector<std::byte> m_Data;
    };
} // namespace Ame
