#pragma once

#include <boost/iostreams/categories.hpp>
#include <ios>

namespace Ame::IO
{
    template<typename CharT, bool OwnsMemory = true>
    class BasicMemDevice
    {
    public:
        using container_type = std::conditional_t<OwnsMemory, std::unique_ptr<CharT>, CharT*>;
        using char_type      = CharT;
        using category       = boost::iostreams::seekable_device_tag;

        BasicMemDevice() = default;

        template<bool = OwnsMemory>
            requires(OwnsMemory)
        BasicMemDevice(
            CharT* container,
            size_t size) :
            m_Container(std::move(container)),
            m_Size(size)
        {
        }

        template<bool = OwnsMemory>
            requires(!OwnsMemory)
        BasicMemDevice(
            size_t size) :
            m_Container(std::make_unique<CharT[]>(size)),
            m_Size(size)
        {
        }

    public:
        std::streamsize read(
            CharT*          s,
            std::streamsize n)
        {
            const auto bytesToRead = std::min(n, static_cast<std::streamsize>(m_Size - m_Position));
            if (bytesToRead == 0)
            {
                return -1;
            }
            else
            {
                std::copy_n(m_Container.get() + m_Position, bytesToRead, s);
                m_Position += bytesToRead;
                return bytesToRead;
            }
        }

        std::streamsize write(
            const CharT*    s,
            std::streamsize n)
        {
            const auto bytesToWrite = std::min(n, static_cast<std::streamsize>(m_Size - m_Position));
            if (bytesToWrite == 0)
            {
                return -1;
            }
            else
            {
                std::copy_n(s, bytesToWrite, m_Container.get() + m_Position);
                m_Position += bytesToWrite;
                return bytesToWrite;
            }
        }

        std::streampos seek(
            std::streamsize        off,
            std::ios_base::seekdir way)
        {
            switch (way)
            {
            case std::ios_base::beg:
                m_Position = off;
                break;
            case std::ios_base::cur:
                m_Position += off;
                break;
            case std::ios_base::end:
                m_Position = m_Size + off - 1;
                break;
            default:
                std::unreachable();
            }

            return m_Position;
        }

        [[nodiscard]] const CharT* data() const
        {
            return m_Container.get();
        }

        [[nodiscard]] CharT* data()
        {
            return m_Container.get();
        }

        [[nodiscard]] size_t size() const
        {
            return m_Size;
        }

    private:
        container_type m_Container;
        size_t         m_Size;
        size_t         m_Position = 0;
    };

    template<typename CharT, bool OwnsMemory = true>
    class BasicMemSource : private BasicMemDevice<CharT, OwnsMemory>
    {
    public:
        using device_type = BasicMemDevice<CharT, OwnsMemory>;
        using char_type   = typename device_type::char_type;
        using category    = boost::iostreams::input_seekable;

    public:
        using device_type::device_type;

        using device_type::read;
        using device_type::seek;
    };

    template<typename CharT, bool OwnsMemory = true>
    class BasicMemSink : private BasicMemDevice<CharT, OwnsMemory>
    {
    public:
        using device_type = BasicMemDevice<CharT, OwnsMemory>;
        using char_type   = typename device_type::char_type;
        using category    = boost::iostreams::output_seekable;

    public:
        using device_type::device_type;

        using device_type::seek;
        using device_type::write;
    };

    //

    using MemDevice     = BasicMemDevice<char, true>;
    using MemDeviceView = BasicMemDevice<char, false>;

    using MemSource     = BasicMemSource<char, true>;
    using MemSourceView = BasicMemSource<char, false>;

    using MemSink     = BasicMemSink<char, true>;
    using MemSinkView = BasicMemSink<char, false>;

    //

    using WMemDevice     = BasicMemDevice<wchar_t, true>;
    using WMemDeviceView = BasicMemDevice<wchar_t, false>;

    using WMemSource     = BasicMemSource<wchar_t, false>;
    using WMemSourceView = BasicMemSource<wchar_t, false>;

    using WMemSink     = BasicMemSink<wchar_t, true>;
    using WMemSinkView = BasicMemSink<wchar_t, false>;
} // namespace Ame::IO
