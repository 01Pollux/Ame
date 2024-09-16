#pragma once

#include <vector>
#include <Core/Interface.hpp>
#include <DiligentCore/Primitives/interface/DataBlob.h>

namespace Ame
{
    using IDataBlob = Dg::IDataBlob;

    // {F578FF0D-ABD2-4514-9D32-7CB454D4A73B}
    inline constexpr UId IID_DataBlob{ 0xf578ff0d, 0xabd2, 0x4514, { 0x9d, 0x32, 0x7c, 0xb4, 0x54, 0xd4, 0xa7, 0x3b } };

    // {74063704-A586-47A5-9349-DE2AC9370DFC}
    inline constexpr UId IID_RawDataBlob{
        0x74063704, 0xa586, 0x47a5, { 0x93, 0x49, 0xde, 0x2a, 0xc9, 0x37, 0xd, 0xfc }
    };

    class RawDataBlob : public BaseObject<IDataBlob>
    {
    public:
        using Base = BaseObject<IDataBlob>;

        IMPLEMENT_QUERY_INTERFACE2_IN_PLACE(IID_RawDataBlob, IID_DataBlob, Base);

    private:
        IMPLEMENT_INTERFACE_CTOR(RawDataBlob, const std::byte* data = nullptr, size_t initialSize = 0) : Base(counters)
        {
            m_Data.resize(initialSize);
            if (data != nullptr)
            {
                std::copy(data, data + initialSize, m_Data.begin());
            }
        }

        IMPLEMENT_INTERFACE_CTOR(RawDataBlob, const IDataBlob* otherBlob) :
            RawDataBlob(counters, std::bit_cast<const std::byte*>(otherBlob->GetConstDataPtr()), otherBlob->GetSize())
        {
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

        template<typename Ty> [[nodiscard]] Ty* GetDataPtr()
        {
            return std::bit_cast<Ty*>(GetDataPtr());
        }

        template<typename Ty> [[nodiscard]] const Ty* GetConstDataPtr() const
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
