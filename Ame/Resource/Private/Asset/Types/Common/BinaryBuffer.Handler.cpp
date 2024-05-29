#include <Asset/Types/Common/BinaryBuffer.Handle.hpp>

namespace Ame::Asset::Common
{
    // https://stackoverflow.com/questions/48964928/read-file-into-stdvectorstdbyte
    struct istreambuf_iterator_byte : public std::istreambuf_iterator<char>
    {
        using base_type = std::istreambuf_iterator<char>;

        using value_type = std::byte;
        using reference  = std::byte;

        std::byte operator*() const noexcept(noexcept(base_type::operator*()))
        {
            return static_cast<std::byte>(base_type::operator*());
        }
        istreambuf_iterator_byte& operator++() noexcept(noexcept(base_type::operator++()))
        {
            base_type::operator++();
            return *this;
        }
        istreambuf_iterator_byte operator++(int) noexcept(noexcept(base_type::operator++(int{})))
        {
            return istreambuf_iterator_byte{ base_type::operator++(int{}) };
        }
    };

    bool BinaryBufferAsset::Handler::CanHandle(
        const Ptr<IAsset>& asset)
    {
        return dynamic_cast<BinaryBufferAsset*>(asset.get());
    }

    Ptr<IAsset> BinaryBufferAsset::Handler::Load(
        std::istream& stream,
        const Asset::DependencyReader&,
        const Guid& guid,
        String      path,
        const AssetMetaData&)
    {
        std::vector<std::byte> buffer(
            istreambuf_iterator_byte(stream),
            istreambuf_iterator_byte{});
        return std::make_shared<BinaryBufferAsset>(std::move(buffer), guid, std::move(path));
    }

    void BinaryBufferAsset::Handler::Save(
        std::iostream& Stream,
        DependencyWriter&,
        const Ptr<IAsset>& Asset,
        AssetMetaData&)
    {
        auto asset = dynamic_cast<const BinaryBufferAsset*>(Asset.get());
        Stream.write(std::bit_cast<const char*>(asset->GetData()), asset->GetSize());
    }
} // namespace Ame::Asset::Common
