#include <Engine/Engine.hpp>

#include <Asset/Subsystem/Storage.hpp>
#include <Gfx/Subsystem/Renderer.hpp>
#include <Gfx/Subsystem/ShaderCache.hpp>

#include <Asset/Types/Common/TextFile.Handler.hpp>
#include <Asset/Types/Common/BinaryBuffer.Handle.hpp>

#include <Asset/Types/Gfx/ShaderSourceAsset.Handler.hpp>

namespace Ame
{
    void BaseEngine::Initialize()
    {
        auto& AssetStorage = GetSubsystem<Asset::StorageSubsystem>();

        AssetStorage.RegisterHandler<Asset::Common::TextFileAsset>();
        AssetStorage.RegisterHandler<Asset::Common::BinaryBufferAsset>();

        if (HasSubsystem<Gfx::RendererSubsystem>())
        {
            AssetStorage.RegisterHandler<Asset::Gfx::ShaderSourceAsset>(
                GetSubsystem<Gfx::Cache::ShaderCacheSubsystem>());
        }
    }
} // namespace Ame
