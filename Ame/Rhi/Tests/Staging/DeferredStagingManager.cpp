#define BOOST_TEST_MODULE PropertyDescriptor
#include <boost/test/included/unit_test.hpp>

#include <Rhi/Device/Device.hpp>

#include <Rhi/Device/Device.hpp>
#include <Rhi/Device/CreateDesc.hpp>
#include <Rhi/CommandList/CommandList.hpp>

#include <Rhi/Staging/DeferredStagingManager.hpp>

BOOST_AUTO_TEST_SUITE(Staging)

namespace Rhi     = Ame::Rhi;
namespace Staging = Ame::Rhi::Staging;

[[nodiscard]] Ame::Co::generator<Rhi::Device> GetDevices()
{
    Rhi::DeviceCreateDesc desc;
    desc.SetFirstAdapter();

    desc.Type = Rhi::DeviceType::DirectX12;
    co_yield Rhi::Device(desc);

    // desc.Type = Rhi::DeviceType::Vulkan;
    // co_yield Rhi::Device(desc);
}

BOOST_AUTO_TEST_CASE(AutoCopyBuffer)
{
    for (auto&& device : GetDevices())
    {
        static constexpr size_t c_BufferSize = 1024;

        Staging::StagedBuffer stagedBuffer(device, Staging::StagedAccessType::Write, { .size = c_BufferSize });
        Rhi::Buffer           buffer(device, Rhi::MemoryLocation::DEVICE, { .size = c_BufferSize });
        Staging::StagedBuffer readbackBuffer(device, Staging::StagedAccessType::Read, { .size = c_BufferSize });

        Staging::DeferredStagingManager manager(device);

        struct SomeStruct
        {
            uint32_t a;
            uint32_t b;
            uint32_t c;
            uint32_t d;
        };

        for (uint32_t i = 0; i < c_BufferSize / sizeof(SomeStruct); ++i)
        {
            SomeStruct data{ i, i + 1, i + 2, i + 3 };
            auto       ptr = stagedBuffer.GetBuffer().GetPtr();
            std::memcpy(std::bit_cast<SomeStruct*>(ptr) + i, &data, sizeof(SomeStruct));
        }

        device.BeginFrame();
        {
            manager.QueueBufferCopy(
                { { .NriBuffer = stagedBuffer.GetBuffer().Unwrap() } },
                { { .NriBuffer = buffer.Unwrap() } });
            manager.Flush();

            manager.QueueBufferCopy(
                { { .NriBuffer = buffer.Unwrap() } },
                { { .NriBuffer = readbackBuffer.GetBuffer().Unwrap() } });
            manager.Flush();
        }
        device.EndFrame();
        device.WaitIdle();

        for (uint32_t i = 0; i < c_BufferSize / sizeof(SomeStruct); ++i)
        {
            SomeStruct data{ i, i + 1, i + 2, i + 3 };
            auto       ptr = readbackBuffer.GetBuffer().GetPtr();
            BOOST_TEST(std::memcmp(std::bit_cast<SomeStruct*>(ptr) + i, &data, sizeof(SomeStruct)) == 0);
        }

        BOOST_TEST(std::memcmp(stagedBuffer.GetBuffer().GetPtr(), readbackBuffer.GetBuffer().GetPtr(), c_BufferSize) == 0);
    }
}

BOOST_AUTO_TEST_CASE(AutoUploadReadbackManyMips)
{
    static constexpr Rhi::Dim_t c_TextureWidth  = 20;
    static constexpr Rhi::Dim_t c_TextureHeight = 20;
    static constexpr Rhi::Dim_t c_MaxArrayCount = 4;
    static constexpr Rhi::Mip_t c_MaxMipLevels  = Rhi::TexMipCount(c_TextureWidth, c_TextureHeight);

    constexpr auto leftSideColor  = Ame::Colors::c_Red;
    constexpr auto rightSideColor = Ame::Colors::c_Blue;

    BOOST_TEST(sizeof(leftSideColor) == sizeof(Ame::Math::Color4));
    BOOST_TEST(sizeof(leftSideColor) == sizeof(rightSideColor));
    BOOST_TEST(sizeof(leftSideColor) == sizeof(float[4]));

    for (auto&& device : GetDevices())
    {
        for (uint32_t arrayCount = 1; arrayCount <= c_MaxArrayCount; arrayCount++)
        {
            for (uint32_t mipCount = 1; mipCount <= c_MaxMipLevels; mipCount++)
            {
                device.WaitIdle();
                const auto textureDesc = Rhi::Tex2D(Rhi::ResourceFormat::RGBA32_SFLOAT, c_TextureWidth, c_TextureHeight, mipCount, arrayCount, Rhi::TextureUsageBits::NONE);

                Staging::StagedTexture stagedTexture(device, textureDesc, Staging::StagedAccessType::Write);
                Rhi::Texture           texture(device, Rhi::MemoryLocation::DEVICE, textureDesc);
                Staging::StagedTexture readbackTexture(device, textureDesc, Staging::StagedAccessType::Read);

                Staging::DeferredStagingManager manager(device);

                for (Rhi::Dim_t arr = 0; arr < textureDesc.arraySize; arr++)
                {
                    for (Rhi::Mip_t mip = 0; mip < textureDesc.mipNum; mip++)
                    {
                        auto& region = readbackTexture.GetRegion(mip, arr);
                        for (uint32_t y = 0; y < region.Height; y++)
                        {
                            for (uint32_t x = 0; x < region.Width; x++)
                            {
                                auto&  color  = (x < (region.Width / 2)) ? leftSideColor : rightSideColor;
                                auto   ptr    = stagedTexture.GetBuffer().GetPtr();
                                size_t offset = region.OffsetAt(x, y);
                                std::memcpy(std::bit_cast<Ame::Math::Color4*>(ptr + offset), &color, sizeof(color));
                            }
                        }
                    }
                }

                device.BeginFrame();
                {
                    for (Rhi::Dim_t arr = 0; arr < textureDesc.arraySize; arr++)
                    {
                        for (Rhi::Mip_t mip = 0; mip < textureDesc.mipNum; mip++)
                        {
                            auto& region = stagedTexture.GetRegion(mip, arr);
                            manager.QueueUpload(
                                { { .NriTexture = texture.Unwrap(),
                                    .NriBuffer  = stagedTexture.GetBuffer().Unwrap(),
                                    .TextureRegion{
                                        .width       = region.Width,
                                        .height      = region.Height,
                                        .depth       = region.Depth,
                                        .mipOffset   = mip,
                                        .arrayOffset = arr },
                                    .BufferOffset = region.Offset } });
                        }
                    }
                    manager.Flush();

                    for (Rhi::Dim_t arr = 0; arr < textureDesc.arraySize; arr++)
                    {
                        for (Rhi::Mip_t mip = 0; mip < textureDesc.mipNum; mip++)
                        {
                            auto& region = readbackTexture.GetRegion(mip, arr);
                            manager.QueueReadback(
                                { { .NriTexture = texture.Unwrap(),
                                    .NriBuffer  = readbackTexture.GetBuffer().Unwrap(),
                                    .TextureRegion{
                                        .width       = region.Width,
                                        .height      = region.Height,
                                        .depth       = region.Depth,
                                        .mipOffset   = mip,
                                        .arrayOffset = arr },
                                    .BufferOffset = region.Offset } });
                        }
                    }
                    manager.Flush();
                }
                device.EndFrame();
                device.WaitIdle();

                //

                BOOST_TEST(std::memcmp(stagedTexture.GetBuffer().GetPtr(), readbackTexture.GetBuffer().GetPtr(), stagedTexture.GetBufferSize()) == 0);
            }
        }
    }
}

BOOST_AUTO_TEST_SUITE_END()