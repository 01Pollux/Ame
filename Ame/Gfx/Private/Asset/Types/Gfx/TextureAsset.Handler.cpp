#include <map>

#include <Asset/Types/Gfx/TextureAsset.Handler.hpp>
#include <Asset/Types/Gfx/TextureAsset.FreeImage.hpp>

#include <Rhi/CommandList/CommandList.hpp>
#include <Rhi/Staging/DeferredStagingManager.hpp>

#include <Log/Wrapper.hpp>

namespace Ame::Asset::Gfx
{
    using FreeImageInstance = Plugins::FreeImageInstance;

    TextureAsset::Handler::Handler(
        Rhi::Device&                          rhiDevice,
        Rhi::Staging::DeferredStagingManager& stagingManager,
        Plugins::FreeImageInstance&) :
        m_Device(rhiDevice),
        m_StagingManager(stagingManager)
    {
    }

    bool TextureAsset::Handler::CanHandle(
        const Ptr<IAsset>& Resource)
    {
        return dynamic_cast<TextureAsset*>(Resource.get());
    }

    //

    Ptr<IAsset> TextureAsset::Handler::Load(
        std::istream&           stream,
        const DependencyReader& dependencyReader,
        const Guid&             guid,
        String                  path,
        const AssetMetaData&    loaderData)
    {
        String extension = GetExtension(path, loaderData);

        auto type = FreeImageInstance::FormatFromExtension(extension);
        if (type == FIF_UNKNOWN)
        {
            return nullptr;
        }
        auto format = FreeImageToImageFormat(type);

        auto freeImageIo = FreeImageInstance::GetIO();
        auto image       = FreeImageInstance::BitmapUPtr(FreeImage_LoadFromHandle(type, &freeImageIo, &stream));
        if (!image)
        {
            return nullptr;
        }
        if (!FreeImage_FlipVertical(image.get()))
        {
            Log::Gfx().Warning("Failed to flip image vertically (Asset: {})", guid.ToString());
        }

        auto image32 = FreeImageInstance::BitmapUPtr(FreeImage_ConvertToRGBAF(image.get()));
        if (!image32)
        {
            return nullptr;
        }

        image.reset();

        uint32_t width  = FreeImage_GetWidth(image32.get());
        uint32_t height = FreeImage_GetHeight(image32.get());

        uint32_t rowPitch = FreeImage_GetPitch(image32.get());

        const std::byte* data = std::bit_cast<const std::byte*>(FreeImage_GetBits(image32.get()));

        // TODO: customize texture desc
        auto textureDesc = Rhi::Tex2D(Rhi::ResourceFormat::RGBA32_SFLOAT, static_cast<Rhi::Dim_t>(width), static_cast<Rhi::Dim_t>(height));
        auto texture     = std::make_shared<Rhi::Texture>(m_Device, Rhi::MemoryLocation::DEVICE, textureDesc);

        auto tempTexture = m_StagingManager.get().AllocateTempTexture(
            textureDesc,
            Rhi::Staging::StagedAccessType::Write);

        auto& region = tempTexture.GetRegion();
        for (Rhi::Dim_t z = 0; z < region.Depth; z++)
        {
            for (Rhi::Dim_t y = 0; y < region.Height; y++)
            {
                size_t offset = region.OffsetAt(0, y);
                std::memcpy(tempTexture.GetBuffer().GetPtr(offset), std::bit_cast<const std::byte*>(data + y * rowPitch), region.RowPitch);
            }
        }

        m_StagingManager.get().QueueUpload(
            { { .NriTexture = texture->Unwrap(),
                .NriBuffer  = tempTexture.GetBuffer().Unwrap(),
                .TextureRegion{
                    .width  = textureDesc.width,
                    .height = textureDesc.height,
                    .depth  = textureDesc.depth },
                .BufferOffset = region.Offset },
              { .PreserveSrcState = false,
                .PreserveDstState = true } });

        Rhi::CommandList commandList(m_Device);
        commandList.RequireState(texture->Unwrap(), { Rhi::AccessBits::SHADER_RESOURCE, Rhi::LayoutType::SHADER_RESOURCE, Rhi::StageBits::GRAPHICS_SHADERS | Rhi::StageBits::COMPUTE_SHADER });

        return std::make_shared<TextureAsset>(std::move(texture), format, guid, path);
    }

    //

    void TextureAsset::Handler::Save(
        std::iostream&     stream,
        DependencyWriter&  dependencyWriter,
        const Ptr<IAsset>& asset,
        AssetMetaData&     loaderData)
    {
        throw std::runtime_error("Not implemented");
    }

    //

    String TextureAsset::Handler::GetExtension(
        String&              path,
        const AssetMetaData& loaderData)
    {
        String extension;
        if (auto forceExtension = loaderData.get_optional<String>("ForceExtension"))
        {
            extension  = std::move(*forceExtension);
            size_t pos = path.find_last_of('.');
            if (pos != path.npos)
            {
                path = path.substr(0, pos) + extension;
            }
            else
            {
                path += extension;
            }
        }
        else
        {
            auto pos = path.find_last_of('.');
            if (pos != path.npos)
            {
                extension = path.substr(pos);
            }
        }
        return extension;
    }
} // namespace Ame::Asset::Gfx