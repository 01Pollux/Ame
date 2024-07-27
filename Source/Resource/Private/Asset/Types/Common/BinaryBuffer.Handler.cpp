#include <Asset/Types/Common/BinaryBuffer.Handler.hpp>

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

    Co::result<Ptr<IAsset>> BinaryBufferAsset::Handler::Load(
        AssetHandlerLoadDesc& loadDesc)
    {
        auto buffer = loadDesc.BackgroundExecutor
                          ->submit(
                              [&]
                              {
                                  std::vector<std::byte> buffer(
                                      istreambuf_iterator_byte(loadDesc.Stream.get()),
                                      istreambuf_iterator_byte{});
                                  return buffer;
                              })
                          .get();

        co_return std::make_shared<BinaryBufferAsset>(std::move(buffer), loadDesc.Guid, std::move(loadDesc.Path));
    }

    Co::result<void> BinaryBufferAsset::Handler::Save(
        AssetHandlerSaveDesc& saveDesc)
    {
        auto asset = dynamic_cast<const BinaryBufferAsset*>(saveDesc.Asset.get());
        saveDesc.BackgroundExecutor
            ->submit(
                [&]
                {
                    saveDesc.Stream.get().write(std::bit_cast<const char*>(asset->GetData()), asset->GetSize());
                })
            .wait();
        co_return;
    }
} // namespace Ame::Asset::Common
