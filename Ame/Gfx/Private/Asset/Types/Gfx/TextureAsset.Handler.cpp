#include <map>

#include <Asset/Types/Gfx/TextureAsset.Handler.hpp>
#include <Asset/Types/Gfx/TextureAsset.FreeImage.hpp>

#include <Rhi/Device/Device.hpp>
#include <Rhi/Device/ResourceAllocator.hpp>
#include <Rhi/Device/CommandSubmitter.hpp>
#include <Rhi/Staging/StagedTexture.hpp>

#include <Log/Wrapper.hpp>

namespace Ame::Asset::Gfx
{
    using FreeImageInstance = Extensions::FreeImageInstance;

    TextureAsset::Handler::Handler(
        Rhi::Device& rhiDevice,
        Extensions::FreeImageInstance&) :
        m_Device(rhiDevice)
    {
    }

    bool TextureAsset::Handler::CanHandle(
        const Ptr<IAsset>& Resource)
    {
        return dynamic_cast<TextureAsset*>(Resource.get());
    }

    //

    [[nodsciard]] static String GetExtension(
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

    //

    static std::pair<FREE_IMAGE_FORMAT, ImageFormat> GetImageTypeFormat(
        const String& extension)
    {
        auto type = FreeImageInstance::FormatFromExtension(extension);
        if (type == FIF_UNKNOWN)
        {
            return {};
        }

        return { type, FreeImageToImageFormat(type) };
    }

    //

    [[nodsciard]] static Co::result<FreeImageInstance::BitmapUPtr> LoadImage(
        FREE_IMAGE_FORMAT     type,
        AssetHandlerLoadDesc& loadDesc)
    {
        auto freeImageIo = FreeImageInstance::GetIO();
        auto image       = FreeImageInstance::BitmapUPtr(FreeImage_LoadFromHandle(type, &freeImageIo, &loadDesc.Stream));
        if (!image)
        {
            co_return nullptr;
        }
        if (!FreeImage_FlipVertical(image.get()))
        {
            Log::Gfx().Warning("Failed to flip image vertically (Asset: {})", loadDesc.Guid.ToString());
        }

        // TODO: customize texture desc
        auto image32 = FreeImageInstance::BitmapUPtr(FreeImage_ConvertToRGBAF(image.get()));
        co_return image32;
    }

    //

    [[nodiscard]] static Rhi::Staging::ScopedStagedTexture CreateUploadTexture(
        Rhi::Device& device,
        FIBITMAP*    image)
    {
        auto& resourceAllocator = device.GetResourceAllocator();

        auto width    = FreeImage_GetWidth(image);
        auto height   = FreeImage_GetHeight(image);
        auto rowPitch = FreeImage_GetPitch(image);
        auto data     = std::bit_cast<const std::byte*>(FreeImage_GetBits(image));

        auto textureDesc = Rhi::Tex2D(Rhi::ResourceFormat::RGBA32_SFLOAT, static_cast<Rhi::Dim_t>(width), static_cast<Rhi::Dim_t>(height));

        Rhi::Staging::ScopedStagedTexture uploadTexture{
            Rhi::Staging::ScopedStagedTexture::Create(resourceAllocator, textureDesc, Rhi::Staging::StagedAccessType::Write)
        };

        auto& region = uploadTexture.GetRegion();
        for (Rhi::Dim_t z = 0; z < region.Depth; z++)
        {
            for (Rhi::Dim_t y = 0; y < region.Height; y++)
            {
                size_t offset = region.OffsetAt(0, y, z);
                std::memcpy(uploadTexture.GetPtr(offset), std::bit_cast<const std::byte*>(data + y * rowPitch), rowPitch);
            }
        }

        return uploadTexture;
    }

    //

    [[nodiscard]] void UploadTexture(
        Rhi::Device&                 rhiDevice,
        Rhi::Texture&                texture,
        const Rhi::TextureDesc&      textureDesc,
        Rhi::Staging::StagedTexture& uploadTexture)
    {
        auto& commandSubmitter = rhiDevice.GetCommandSubmitter();

        auto submission = commandSubmitter.BeginCommandList(Rhi::CommandQueueType::GRAPHICS);

        {
            Rhi::TextureBarrierDesc textureBarrier[]{
                { .texture = texture.Unwrap(),
                  .before{ Rhi::AccessBits::UNKNOWN, Rhi::LayoutType::UNKNOWN, Rhi::StageBits::ALL },
                  .after{ Rhi::AccessBits::COPY_DESTINATION, Rhi::LayoutType::COPY_DESTINATION, Rhi::StageBits::COPY } }
            };
            Rhi::BufferBarrierDesc bufferBarrier[]{
                { .buffer = uploadTexture.Unwrap(),
                  .before{ Rhi::AccessBits::UNKNOWN, Rhi::StageBits::ALL },
                  .after{ Rhi::AccessBits::COPY_SOURCE, Rhi::StageBits::COPY } }
            };
            Rhi::BarrierGroupDesc barrier{
                .buffers    = bufferBarrier,
                .textures   = textureBarrier,
                .bufferNum  = 1,
                .textureNum = 1,
            };

            submission->ResourceBarrier(barrier);
        }

        {
            Rhi::MarkerCommand uploadMarker(submission.CommandListRef, "Upload Texture");

            submission->UploadTexture(
                rhiDevice.GetDesc(), { .NriTexture = texture.Unwrap(),
                                       .NriBuffer  = uploadTexture.Unwrap(),
                                       .TextureRegion{
                                           .width  = textureDesc.width,
                                           .height = textureDesc.height,
                                           .depth  = textureDesc.depth } });
        }

        {
            Rhi::TextureBarrierDesc textureBarrier[]{
                { .texture = texture.Unwrap(),
                  .before{ Rhi::AccessBits::COPY_DESTINATION, Rhi::LayoutType::COPY_DESTINATION, Rhi::StageBits::COPY },
                  .after{ Rhi::AccessBits::SHADER_RESOURCE, Rhi::LayoutType::SHADER_RESOURCE, Rhi::StageBits::GRAPHICS_SHADERS | Rhi::StageBits::COMPUTE_SHADER } }
            };
            Rhi::BarrierGroupDesc barrier{
                .textures   = textureBarrier,
                .textureNum = 1,
            };

            submission->ResourceBarrier(barrier);
        }

        commandSubmitter.SubmitCommandList(submission).wait();
    }

    //

    Co::result<Ptr<IAsset>> TextureAsset::Handler::Load(
        AssetHandlerLoadDesc& loadDesc)
    {
        auto& resourceAllocator = m_Device.get().GetResourceAllocator();
        auto  uploadExecutor    = m_Device.get().GetExecutor(Rhi::ExecutorType::Copy);

        String extension    = GetExtension(loadDesc.Path, loadDesc.LoaderData);
        auto [type, format] = GetImageTypeFormat(extension);

        co_await Co::resume_on(loadDesc.BackgroundExecutor);

        auto image = co_await LoadImage(type, loadDesc);

        co_await Co::resume_on(loadDesc.ForegroundExecutor);

        // TODO: maybe return dev null texture
        if (image)
        {
            co_return nullptr;
        }

        uint32_t width    = FreeImage_GetWidth(image.get());
        uint32_t height   = FreeImage_GetHeight(image.get());
        uint32_t rowPitch = FreeImage_GetPitch(image.get());
        auto     data     = std::bit_cast<const std::byte*>(FreeImage_GetBits(image.get()));

        // TODO: handle format
        auto textureDesc = Rhi::Tex2D(Rhi::ResourceFormat::RGBA32_SFLOAT, static_cast<Rhi::Dim_t>(width), static_cast<Rhi::Dim_t>(height));
        auto texture     = std::make_shared<Rhi::ScopedTexture>(resourceAllocator.CreateTexture(textureDesc));

        auto uploadTask = co_await uploadExecutor->submit(
                                                     [&]
                                                     {
                                                         auto uploadTexture = CreateUploadTexture(m_Device, image.get());
                                                         UploadTexture(m_Device, *texture, textureDesc, uploadTexture);
                                                     })
                              .resolve();

        co_await uploadTask;

        co_return std::make_shared<TextureAsset>(std::move(texture), format, std::move(loadDesc.Guid), std::move(loadDesc.Path));
    }

    //

    Co::result<void> TextureAsset::Handler::Save(
        AssetHandlerSaveDesc& saveDesc)
    {
        throw std::runtime_error("Not implemented");
    }
} // namespace Ame::Asset::Gfx