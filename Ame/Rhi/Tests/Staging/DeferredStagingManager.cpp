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

[[nodiscard]] Rhi::Device GetDevice()
{
    Rhi::DeviceCreateDesc desc;
    desc.SetFirstAdapter();
    desc.Type = Rhi::DeviceType::DirectX12;

    return Rhi::Device(desc);
}

BOOST_AUTO_TEST_CASE(AutoCopyBuffer)
{
    auto device = GetDevice();

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

    //

    for (uint32_t i = 0; i < c_BufferSize / sizeof(SomeStruct); ++i)
    {
        SomeStruct data{ i, i + 1, i + 2, i + 3 };
        auto       ptr = readbackBuffer.GetBuffer().GetPtr();
        BOOST_TEST(std::memcmp(std::bit_cast<SomeStruct*>(ptr) + i, &data, sizeof(SomeStruct)) == 0);
    }

    BOOST_TEST(std::memcmp(stagedBuffer.GetBuffer().GetPtr(), readbackBuffer.GetBuffer().GetPtr(), c_BufferSize) == 0);
}

BOOST_AUTO_TEST_CASE(AutoUploadReadback)
{
    auto device = GetDevice();

    static constexpr auto textureDesc = Rhi::Tex2D(Rhi::ResourceFormat::RGBA32_SFLOAT, 1024, 720, 1);

    Staging::StagedTexture stagedTexture(device, textureDesc, Staging::StagedAccessType::Write);
    Rhi::Texture           texture(device, Rhi::MemoryLocation::DEVICE, textureDesc);
    Staging::StagedTexture readbackTexture(device, textureDesc, Staging::StagedAccessType::Read);

    Staging::DeferredStagingManager manager(device);

    constexpr auto leftSideColor  = Ame::Colors::c_Green;
    constexpr auto rightSideColor = Ame::Colors::c_Yellow;

    BOOST_TEST(sizeof(leftSideColor) == sizeof(Ame::Math::Color4));
    BOOST_TEST(sizeof(leftSideColor) == sizeof(rightSideColor));
    BOOST_TEST(sizeof(leftSideColor) == sizeof(float[4]));

    for (uint32_t y = 0; y < textureDesc.height; ++y)
    {
        for (uint32_t x = 0; x < textureDesc.width; ++x)
        {
            auto& color = (x < (textureDesc.width / 2)) ? leftSideColor : rightSideColor;
            auto  ptr   = stagedTexture.GetBuffer().GetPtr();
            std::memcpy(std::bit_cast<Ame::Math::Color4*>(ptr) + (y * textureDesc.width + x), &color, sizeof(color));
        }
    }

    device.BeginFrame();
    {
        manager.QueueUpload(
            { { .NriTexture = texture.Unwrap(),
                .NriBuffer  = stagedTexture.GetBuffer().Unwrap(),
                .TextureRegion{
                    .width  = textureDesc.width,
                    .height = textureDesc.height,
                    .depth  = 1 } } });
        manager.Flush();

        manager.QueueReadback(
            { { .NriTexture = texture.Unwrap(),
                .NriBuffer  = readbackTexture.GetBuffer().Unwrap(),
                .TextureRegion{
                    .width  = textureDesc.width,
                    .height = textureDesc.height,
                    .depth  = 1 } } });
        manager.Flush();
    }
    device.EndFrame();
    device.WaitIdle();

    //

    BOOST_TEST(std::memcmp(stagedTexture.GetBuffer().GetPtr(), readbackTexture.GetBuffer().GetPtr(), stagedTexture.GetBufferSize()) == 0);
}

BOOST_AUTO_TEST_SUITE_END()