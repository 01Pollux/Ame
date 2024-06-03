#include <Gfx/Shading/Material.Compiler.hpp>
#include <Gfx/Shading/Material.hpp>
#include <Rhi/Device/Device.hpp>

#include <Gfx/Constants.hpp>

namespace Ame::Gfx::Shading
{
    namespace CD = Constants::DescriptorRanges;

    //

    struct MaterialLayoutDesc
    {
        static constexpr uint32_t InitialSetCount  = 2;
        static constexpr uint32_t MaterialSetCount = InitialSetCount + 1;

    public:
        explicit MaterialLayoutDesc(
            const PropertyDescriptor& Descriptor) :
            m_FrameData(CD::c_FrameRangeDesc<>),
            m_EntityData(CD::c_EntityRangeDesc<>),
            m_Sets{
                { .registerSpace = CD::c_FrameData_RegisterSpace, .ranges = &m_FrameData, .rangeNum = 1 },
                { .registerSpace = CD::c_EntityData_RegisterSpace, .ranges = &m_EntityData, .rangeNum = 1 },
                { .registerSpace = CD::c_MaterialData_RegisterSpace }
            },
            m_SetCount(InitialSetCount)
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
            const PropertyDescriptor& descriptor)
        {
            if (!descriptor.IsEmpty())
            {
                m_Sets[m_SetCount].dynamicConstantBuffers;
                uint32_t bindingIndex = 0;
                if (descriptor.GetStructSize() > 0)
                {
                    m_UserData.registerIndex = bindingIndex++;
                    m_UserData.shaderStages  = descriptor.GetStructAccessFlags().Flags;
                    m_ShaderBits.Set(m_UserData.shaderStages);

                    m_Sets[m_SetCount].dynamicConstantBuffers   = &m_UserData;
                    m_Sets[m_SetCount].dynamicConstantBufferNum = 1;
                }

                if (descriptor.GetResourceCount() > 0)
                {
                    for (auto& resource : descriptor.GetResources())
                    {
                        Rhi::DescriptorType type = Rhi::DescriptorType::MAX_NUM;
                        switch (resource.get().Type)
                        {
                        case ResourceType::Buffer:
                            type = Rhi::DescriptorType::BUFFER;
                            break;
                        case ResourceType::RWBuffer:
                            type = Rhi::DescriptorType::STORAGE_BUFFER;
                            break;

                        case ResourceType::StructuredBuffer:
                            type = Rhi::DescriptorType::STRUCTURED_BUFFER;
                            break;
                        case ResourceType::RWStructuredBuffer:
                            type = Rhi::DescriptorType::STORAGE_STRUCTURED_BUFFER;
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
                            type = Rhi::DescriptorType::TEXTURE;
                            break;

                        case ResourceType::RWTexture1D:
                        case ResourceType::RWTexture1DArray:
                        case ResourceType::RWTexture2D:
                        case ResourceType::RWTexture2DArray:
                        case ResourceType::RWTexture3D:
                            type = Rhi::DescriptorType::STORAGE_TEXTURE;
                            break;

                        case ResourceType::Sampler:
                            type = Rhi::DescriptorType::SAMPLER;
                            break;

                        default:
                            std::unreachable();
                        }

                        auto shaderFlags = resource.get().ShaderFlags.Flags;
                        m_ShaderBits.Set(shaderFlags);

                        m_ResourceDatas.emplace_back(Rhi::DescriptorRangeDesc{
                            .baseRegisterIndex = bindingIndex++,
                            .descriptorNum     = 1,
                            .descriptorType    = type,
                            .shaderStages      = shaderFlags });
                    }

                    m_Sets[m_SetCount].ranges   = m_ResourceDatas.data();
                    m_Sets[m_SetCount].rangeNum = Rhi::Count32(m_ResourceDatas);
                }

                m_SetCount++;
            }
            else
            {
                m_ShaderBits = Rhi::ShaderFlags::Graphics();
            }

            m_FrameData.shaderStages  = m_ShaderBits.Flags;
            m_EntityData.shaderStages = m_ShaderBits.Flags;
        }

    private:
        Rhi::DescriptorRangeDesc m_FrameData;
        Rhi::DescriptorRangeDesc m_EntityData;

        Rhi::DynamicConstantBufferDesc        m_UserData{};
        std::vector<Rhi::DescriptorRangeDesc> m_ResourceDatas;

        Rhi::DescriptorSetDesc m_Sets[MaterialSetCount];
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
        Rhi::Device&              RhiDevice,
        Gfx::Cache::ShaderCache&  ShaderCache,
        MaterialPipelineState     PipelineState,
        const PropertyDescriptor& Descriptor)
    {
#ifndef AME_DIST
        if (!PipelineState.FindShader(Rhi::ShaderType::VERTEX_SHADER) ||
            !PipelineState.FindShader(Rhi::LibraryShaderType))
        {
            co_return nullptr;
        }
#endif

        auto Layout = co_await CreatePipelineLayout(RhiDevice, Descriptor);
        co_return std::make_shared<Material>(RhiDevice, ShaderCache, std::move(Layout), std::move(PipelineState), Descriptor);
    }
} // namespace Ame::Gfx::Shading