#include <Gfx/Shading/Material.Compiler.hpp>
#include <Gfx/Shading/Material.hpp>
#include <Rhi/Device/Device.hpp>

#include <Gfx/Constants.hpp>

namespace Ame::Gfx::Shading
{
    namespace ConstantDescriptorRanges = Constants::DescriptorRanges;

    //

    struct MaterialLayoutDesc
    {
    public:
        MaterialLayoutDesc(
            const PropertyDescriptor& Descriptor) :
            m_FrameData{ .baseRegisterIndex = ConstantDescriptorRanges::FrameData_RegisterIndex,
                         .descriptorNum     = 1,
                         .descriptorType    = Rhi::DescriptorType::CONSTANT_BUFFER },
            m_Sets{
                { .registerSpace = ConstantDescriptorRanges::FrameData_RegisterSpace, .ranges = &m_FrameData, .rangeNum = 1 },
                { .registerSpace = 20 }
            },
            m_SetCount(1)
        {
            FillDescriptorSet(Descriptor);
        }

    public:
        [[nodiscard]] Rhi::PipelineLayoutDesc GetLayout() const
        {
            return Rhi::PipelineLayoutDesc{
                .descriptorSets                     = m_Sets,
                .descriptorSetNum                   = m_SetCount,
                .shaderStages                       = m_ShaderBits.Flags,
                .enableD3D12DrawParametersEmulation = Rhi::Device::EnableDrawParametersEmulation
            };
        }

    private:
        void FillDescriptorSet(
            const PropertyDescriptor& Descriptor)
        {
            if (!Descriptor.IsEmpty())
            {
                m_Sets[m_SetCount].dynamicConstantBuffers;
                if (Descriptor.GetStructSize() > 0)
                {
                    m_UserData.shaderStages = Descriptor.GetStructAccessFlags().Flags;
                    m_ShaderBits.Set(m_UserData.shaderStages);

                    m_Sets[m_SetCount].dynamicConstantBuffers   = &m_UserData;
                    m_Sets[m_SetCount].dynamicConstantBufferNum = 1;
                }

                if (Descriptor.GetResourceCount() > 0)
                {
                    uint32_t Index = 0;
                    for (auto& Resource : Descriptor.GetResources())
                    {
                        Rhi::DescriptorType Type = Rhi::DescriptorType::MAX_NUM;
                        switch (Resource.get().Type)
                        {
                        case ResourceType::Buffer:
                            Type = Rhi::DescriptorType::BUFFER;
                            break;
                        case ResourceType::RWBuffer:
                            Type = Rhi::DescriptorType::STORAGE_BUFFER;
                            break;

                        case ResourceType::StructuredBuffer:
                            Type = Rhi::DescriptorType::STRUCTURED_BUFFER;
                            break;
                        case ResourceType::RWStructuredBuffer:
                            Type = Rhi::DescriptorType::STORAGE_STRUCTURED_BUFFER;
                            break;

                        case ResourceType::Texture1D:
                        case ResourceType::Texture1DArray:
                        case ResourceType::Texture2D:
                        case ResourceType::Texture2DArray:
                        case ResourceType::Texture2DMS:
                        case ResourceType::Texture2DMSArray:
                        case ResourceType::Texture3D:
                        case ResourceType::TextureCube:
                        case ResourceType::TextureCubeArray:
                            Type = Rhi::DescriptorType::TEXTURE;
                            break;

                        case ResourceType::RWTexture1D:
                        case ResourceType::RWTexture1DArray:
                        case ResourceType::RWTexture2D:
                        case ResourceType::RWTexture2DArray:
                        case ResourceType::RWTexture3D:
                            Type = Rhi::DescriptorType::STORAGE_TEXTURE;
                            break;

                        case ResourceType::Sampler:
                            Type = Rhi::DescriptorType::SAMPLER;
                            break;

                        default:
                            std::unreachable();
                        }

                        auto ShaderFlags = Resource.get().ShaderFlags.Flags;
                        m_ShaderBits.Set(ShaderFlags);

                        m_ResourceDatas.emplace_back(Rhi::DescriptorRangeDesc{
                            .baseRegisterIndex = Index++,
                            .descriptorNum     = 1,
                            .descriptorType    = Type,
                            .shaderStages      = ShaderFlags });
                    }

                    m_Sets[m_SetCount].ranges   = m_ResourceDatas.data();
                    m_Sets[m_SetCount].rangeNum = Rhi::Count32(m_ResourceDatas);
                }

                m_SetCount++;
            }
            else
            {
                m_ShaderBits.Set(Rhi::ShaderType::ALL);
            }

            m_FrameData.shaderStages = m_ShaderBits.Flags;
        }

    private:
        Rhi::DescriptorRangeDesc m_FrameData;

        Rhi::DynamicConstantBufferDesc        m_UserData{};
        std::vector<Rhi::DescriptorRangeDesc> m_ResourceDatas;

        Rhi::DescriptorSetDesc m_Sets[2];
        uint32_t               m_SetCount;

        Rhi::ShaderFlags m_ShaderBits;
    };

    [[nodiscard]] static Co::result<Ptr<Rhi::PipelineLayout>> CreatePipelineLayout(
        Rhi::Device&              RhiDevice,
        const PropertyDescriptor& Descriptor)
    {
        MaterialLayoutDesc MaterialLayout(Descriptor);
        co_return RhiDevice.CreatePipelineLayout(MaterialLayout.GetLayout());
    }

    //

    Co::result<Ptr<Material>> MaterialCompiler::Compile(
        Rhi::Device&                 RhiDevice,
        const MaterialPipelineState& PipelineState,
        const PropertyDescriptor&    Descriptor)
    {
        auto Layout = co_await CreatePipelineLayout(RhiDevice, Descriptor);
        co_return std::make_shared<Material>(RhiDevice, std::move(Layout), PipelineState, Descriptor);
    }
} // namespace Ame::Gfx::Shading