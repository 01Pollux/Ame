#include <Asset/Types/Common/BinaryBuffer.Handle.hpp>

namespace Ame::Asset::Common
{
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
        std::vector<uint8_t> buffer(std::istreambuf_iterator<char>(stream), {});
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
