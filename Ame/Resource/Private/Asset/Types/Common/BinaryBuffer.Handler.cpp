#include <Asset/Types/Common/BinaryBuffer.Handle.hpp>

namespace Ame::Asset::Common
{
    bool BinaryBufferAsset::Handler::CanHandle(
        const Ptr<IAsset>& Resource)
    {
        return dynamic_cast<BinaryBufferAsset*>(Resource.get());
    }

    Ptr<IAsset> BinaryBufferAsset::Handler::Load(
        std::istream&                  Stream,
        const Asset::DependencyReader& DepReader,
        const Handle&                  AssetGuid,
        String                         Path,
        const AssetMetaData&           LoaderData)
    {
        std::vector<uint8_t> Buffer(std::istreambuf_iterator<char>(Stream), {});
        return std::make_shared<BinaryBufferAsset>(std::move(Buffer), AssetGuid, std::move(Path));
    }

    void BinaryBufferAsset::Handler::Save(
        std::iostream& Stream,
        DependencyWriter&,
        const Ptr<IAsset>& Asset,
        AssetMetaData&     LoaderData)
    {
        auto Buffer = dynamic_cast<const BinaryBufferAsset*>(Asset.get());
        Stream.write(std::bit_cast<const char*>(Buffer->GetData()), Buffer->GetSize());
    }
} // namespace Ame::Asset::Common
