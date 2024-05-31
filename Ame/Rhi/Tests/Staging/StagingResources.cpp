#define BOOST_TEST_MODULE PropertyDescriptor
#include <boost/test/included/unit_test.hpp>

#include <Core/Container.hpp>
#include <Rhi/Subsystem/Device.hpp>

#include <Rhi/Device/Device.hpp>
#include <Rhi/Device/CreateDesc.hpp>
#include <Rhi/CommandList/CommandList.hpp>

#include <Rhi/Staging/StagedBuffer.hpp>
#include <Rhi/Staging/StagedTexture.hpp>

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

BOOST_AUTO_TEST_CASE(SimpleBuffer)
{
    auto device = GetDevice();

    static constexpr size_t c_BufferSize = 1024;

    Staging::StagedBuffer stagedBuffer(device, Staging::StagedAccessType::Write, { .size = c_BufferSize });

    auto& desc = stagedBuffer.GetBuffer().GetDesc();
    auto  size = stagedBuffer.GetSize();

    BOOST_TEST(desc.size == size);
    BOOST_TEST(desc.size == c_BufferSize);
}

BOOST_AUTO_TEST_CASE(SimpleTexture1D)
{
    auto device = GetDevice();

    struct FormatAndSize
    {
        Rhi::ResourceFormat format;
        size_t              size;
    };

    constexpr FormatAndSize c_FormatAndSizes[]{
        { Rhi::ResourceFormat::RGBA8_SINT, sizeof(uint8_t[4]) },
        { Rhi::ResourceFormat::RGBA16_SINT, sizeof(uint16_t[4]) },
        { Rhi::ResourceFormat::RGBA32_SINT, sizeof(uint32_t[4]) },
        { Rhi::ResourceFormat::R32_SFLOAT, sizeof(float) },
    };

    for (auto [format, dataSize] : c_FormatAndSizes)
    {
        auto textureDesc = Rhi::Tex1D(format, 1024, 1);

        Staging::StagedTexture stagedTexture(device, textureDesc, Staging::StagedAccessType::Write);
        Rhi::Texture           texture(device, Rhi::MemoryLocation::DEVICE, textureDesc);

        auto& stagedDesc = stagedTexture.GetBuffer().GetDesc();
        auto& desc       = texture.GetDesc();

        BOOST_TEST(stagedDesc.size == (desc.width * desc.height * desc.depth * dataSize));
    }
}

//

BOOST_AUTO_TEST_CASE(ManualCopyBuffer)
{
    auto device = GetDevice();

    Rhi::CommandList commandList(device);

    static constexpr size_t c_BufferSize = 1024;

    Staging::StagedBuffer stagedBuffer(device, Staging::StagedAccessType::Write, { .size = c_BufferSize });
    Rhi::Buffer           buffer(device, Rhi::MemoryLocation::DEVICE, { .size = c_BufferSize });
    Staging::StagedBuffer readbackBuffer(device, Staging::StagedAccessType::Read, { .size = c_BufferSize });

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

    //

    device.BeginFrame();
    {
        commandList.RequireState(stagedBuffer.GetBuffer().Unwrap(), { Rhi::AccessBits::COPY_SOURCE, Rhi::StageBits::COPY });
        commandList.RequireState(buffer.Unwrap(), { Rhi::AccessBits::COPY_DESTINATION, Rhi::StageBits::COPY });
        commandList.CommitBarriers();

        commandList.CopyBuffer(
            { .NriBuffer = stagedBuffer.GetBuffer().Unwrap() },
            { .NriBuffer = buffer.Unwrap() });

        commandList.RequireState(buffer.Unwrap(), { Rhi::AccessBits::COPY_SOURCE, Rhi::StageBits::COPY });
        commandList.RequireState(readbackBuffer.GetBuffer().Unwrap(), { Rhi::AccessBits::COPY_DESTINATION, Rhi::StageBits::COPY });
        commandList.CommitBarriers();

        commandList.CopyBuffer(
            { .NriBuffer = buffer.Unwrap() },
            { .NriBuffer = readbackBuffer.GetBuffer().Unwrap() });

        commandList.RequireState(readbackBuffer.GetBuffer().Unwrap(), { Rhi::AccessBits::UNKNOWN, Rhi::StageBits::NONE });
    }
    device.EndFrame();
    device.WaitIdle();

    //

    BOOST_TEST(std::memcmp(stagedBuffer.GetBuffer().GetPtr(), readbackBuffer.GetBuffer().GetPtr(), c_BufferSize) == 0);
}

BOOST_AUTO_TEST_SUITE_END()