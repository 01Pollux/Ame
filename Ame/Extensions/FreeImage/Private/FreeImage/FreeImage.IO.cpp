#include <FreeImage/FreeImage.hpp>
#include <fstream>

namespace Ame::Extensions
{
    static unsigned FIReadPorc(
        void*     buffer,
        unsigned  size,
        unsigned  count,
        fi_handle handle)
    {
        auto& stream = *std::bit_cast<std::istream*>(handle);
        auto  pos    = stream.tellg();
        stream.read(std::bit_cast<char*>(buffer), size * count);
        return static_cast<unsigned>(stream.tellg() - pos);
    }

    static unsigned FIWriteProc(
        void*     buffer,
        unsigned  size,
        unsigned  count,
        fi_handle handle)
    {
        auto& stream = *std::bit_cast<std::iostream*>(handle);
        auto  pos    = stream.tellp();
        stream.write(std::bit_cast<char*>(buffer), size * count);
        return static_cast<unsigned>(stream.tellp() - pos);
    }

    static int FISetProc(
        fi_handle handle,
        long      offset,
        int       pos)
    {
        auto& stream = *std::bit_cast<std::iostream*>(handle);
        switch (pos)
        {
        case SEEK_SET:
        {
            stream.seekp(offset, std::ios::beg);
            stream.seekg(offset, std::ios::beg);
            break;
        }
        case SEEK_CUR:
        {
            stream.seekp(offset, std::ios::cur);
            stream.seekg(offset, std::ios::cur);
            break;
        }
        case SEEK_END:
        {
            stream.seekp(offset, std::ios::end);
            stream.seekg(offset, std::ios::end);
            break;
        }
        default:
            return 1;
        }
        return 0;
    }

    static long FIGetProc(
        fi_handle handle)
    {
        auto& stream = *std::bit_cast<std::iostream*>(handle);
        return static_cast<long>(stream.tellg());
    }

    //

    FreeImageIO FreeImageInstance::GetIO() noexcept
    {
        return FreeImageIO{
            .read_proc  = FIReadPorc,
            .write_proc = FIWriteProc,
            .seek_proc  = FISetProc,
            .tell_proc  = FIGetProc
        };
    }
} // namespace Ame::Extensions