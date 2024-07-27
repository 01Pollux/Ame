#include <Asset/Types/Common/TextFile.Handler.hpp>

#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>

namespace Ame::Asset::Common
{
    bool TextFileAsset::Handler::CanHandle(
        const Ptr<IAsset>& asset)
    {
        return dynamic_cast<TextFileAsset*>(asset.get());
    }

    Co::result<Ptr<IAsset>> TextFileAsset::Handler::Load(
        AssetHandlerLoadDesc& loadDesc)
    {
        auto text = loadDesc.BackgroundExecutor
                        ->submit(
                            [&]
                            {
                                std::stringstream text;
                                text << loadDesc.Stream.get().rdbuf();
                                return text.str();
                            })
                        .get();

        co_return std::make_shared<TextFileAsset>(std::move(text), loadDesc.Guid, std::move(loadDesc.Path));
    }

    Co::result<void> TextFileAsset::Handler::Save(
        AssetHandlerSaveDesc& saveDesc)
    {
        saveDesc.BackgroundExecutor
			->submit(
				[&]
				{
					auto asset = dynamic_cast<const TextFileAsset*>(saveDesc.Asset.get());
					saveDesc.Stream.get() << asset->Get();
				})
			.wait();
		co_return;
    }
} // namespace Ame::Asset::Common
