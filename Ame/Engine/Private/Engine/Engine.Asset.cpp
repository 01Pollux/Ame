#include <Engine/Engine.hpp>

#include <Asset/Subsystem/Storage.hpp>
#include <Gfx/Subsystem/Renderer.hpp>
#include <Gfx/Subsystem/ShaderCache.hpp>

#include <FreeImage/Subsystem/FreeImage.hpp>

#include <Asset/Types/Common/TextFile.Handler.hpp>
#include <Asset/Types/Common/BinaryBuffer.Handle.hpp>

#include <Asset/Types/Gfx/ShaderSourceAsset.Handler.hpp>
#include <Asset/Types/Gfx/TextureAsset.Handler.hpp>

namespace Ame
{
    void BaseEngine::Initialize()
    {
        auto& assetStorage = GetSubsystem<Asset::StorageSubsystem>();

        assetStorage.RegisterHandler<Asset::Common::TextFileAsset::Handler>();
        assetStorage.RegisterHandler<Asset::Common::BinaryBufferAsset::Handler>();

        if (HasSubsystem<Gfx::RendererSubsystem>())
        {
            assetStorage.RegisterHandler<Asset::Gfx::ShaderSourceAsset::Handler>(
                GetSubsystem<Gfx::Cache::ShaderCacheSubsystem>());
            assetStorage.RegisterHandler<Asset::Gfx::TextureAsset::Handler>(
                GetSubsystem<Rhi::DeviceSubsystem>(),
                GetSubsystem<Rhi::Staging::DeferredStagingManagerSubsystem>(),
                GetSubsystem<Plugins::FreeImageSubsystem>());
        }
    }
} // namespace Ame
