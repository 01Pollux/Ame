#pragma once

#include <vector>
#include <Core/Interface.hpp>
#include <DiligentCore/Primitives/interface/DataBlob.h>

namespace Ame
{
    // {99E53E10-55D1-4E81-871D-09C580060D43}
    inline constexpr UId IID_DataBlob{ 0x99e53e10, 0x55d1, 0x4e81, { 0x87, 0x1d, 0x9, 0xc5, 0x80, 0x6, 0xd, 0x43 } };

    class IDataBlob : public IObjectWithCallback
    {
    public:
        IMPLEMENT_QUERY_INTERFACE_IN_PLACE(IID_DataBlob, IObjectWithCallback);

        using IObjectWithCallback::IObjectWithCallback;

    public:
        virtual void        Resize(size_t newSize)  = 0;
        virtual size_t      GetSize() const         = 0;
        virtual void*       GetDataPtr()            = 0;
        virtual const void* GetConstDataPtr() const = 0;
    };

    // {CD902203-B43C-48CA-9C6A-625AC4C98D5B}
    inline constexpr UId IID_RawDataBlob{
        0xcd902203, 0xb43c, 0x48ca, { 0x9c, 0x6a, 0x62, 0x5a, 0xc4, 0xc9, 0x8d, 0x5b }
    };

    class RawDataBlob : public IDataBlob
    {
    public:
        IMPLEMENT_QUERY_INTERFACE_IN_PLACE(IID_RawDataBlob, IDataBlob);

    private:
        IMPLEMENT_INTERFACE_CTOR(RawDataBlob, const std::byte* data = nullptr, size_t initialSize = 0) :
            IDataBlob(counters)
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
