#pragma once

#include <Rhi/Utils/ShaderCreateInfoX.hpp>
#include <DiligentCore/Graphics/GraphicsTools/interface/ShaderSourceFactoryUtils.hpp>

#include <Log/Wrapper.hpp>

namespace Ame::Rhi
{
    struct PartialShaderDesc
    {
        const Dg::ShaderCreateInfo* Shader = nullptr;
    };

    struct CombinedShaderCreateDesc
    {
        std::span<const PartialShaderDesc> Shaders;
    };

    /// <summary>
    /// A combined shader is a shader that is composed of multiple shaders.
    /// it consists of multiple #includes statements.
    /// The shaders must be provided as source code, bytecode is not supported.
    /// The shaders must have similar setup, such as the same shader type etc.
    /// </summary>
    class CombinedShader : public ShaderCreateInfoX
    {
    public:
        using ShaderCreateInfoX::ShaderCreateInfoX;

    public:
        CombinedShader(
            const CombinedShaderCreateDesc& desc)
        {
            Initialize(desc);
        }

        void Initialize(
            const CombinedShaderCreateDesc& desc)
        {
            if (desc.Shaders.empty())
            {
                return;
            }

            std::vector<Dg::IShaderSourceInputStreamFactory*> factories;
            std::vector<Dg::MemoryShaderSourceFileInfo>       memorySources;
            std::vector<String>                               paths;

            memorySources.reserve(desc.Shaders.size());
            paths.reserve(desc.Shaders.size());

            String sourceCode;
            for (auto& curDesc : desc.Shaders)
            {
                Validate(curDesc);
                auto& [shader] = curDesc;

                auto& path = paths.emplace_back( std::format("__Composer/{}.aps", static_cast<const void*>(shader)));
                sourceCode.append(std::format("#include \"{}\"\n", path));

                AppendMacros({ shader->Macros.Elements, shader->Macros.Count });
                if (shader->pShaderSourceStreamFactory)
                {
                    factories.push_back(shader->pShaderSourceStreamFactory);
                }

                memorySources.emplace_back(path.c_str(), shader->Source, static_cast<uint32_t>(shader->SourceLength));

                CompileFlags(CompileFlags() | shader->CompileFlags);
                LoadConstantBufferReflection(LoadConstantBufferReflection() || shader->LoadConstantBufferReflection);

                if (shader->GLSLExtensions)
                {
                    AppendGLSLExtensions(shader->GLSLExtensions);
                }
                if (shader->WebGPUEmulatedArrayIndexSuffix)
                {
                    WebGPUEmulatedArrayIndexSuffix(shader->WebGPUEmulatedArrayIndexSuffix);
                }

                ShaderType(shader->Desc.ShaderType);
            }

            SourceCode(std::move(sourceCode));
            CreateSourceStream(factories, memorySources);
        }

        void Reset()
        {
            SourceCodeCStr(nullptr);
            SourceInputStreamFactory({});
        }

    private:
        void CreateSourceStream(
            std::vector<Dg::IShaderSourceInputStreamFactory*>& factories,
            std::span<const Dg::MemoryShaderSourceFileInfo>    memorySources)
        {
            Ptr<Dg::IShaderSourceInputStreamFactory> memoryFactory;
            Dg::CreateMemoryShaderSourceFactory({ memorySources.data(), Count32(memorySources) }, &memoryFactory);
            if (factories.empty())
            {
                SourceInputStreamFactory(std::move(memoryFactory));
            }
            else
            {
                factories.push_back(memoryFactory);
                Ptr<Dg::IShaderSourceInputStreamFactory> combinedFactory;
                Dg::CreateCompoundShaderSourceFactory({ factories.data(), Count32(factories) }, &combinedFactory);
                SourceInputStreamFactory(std::move(combinedFactory));
            }
        }

    private:
        void Validate(
            const PartialShaderDesc& desc)
        {
#ifdef AME_DEBUG
            Log::Rhi().Assert(desc.Shader, "Shader is null");
            Log::Rhi().Assert(desc.Shader->Source, "Source is null");
            Log::Rhi().Assert(!desc.Shader->ByteCode, "Byte code is not supported");
            Log::Rhi().Assert(desc.Shader->SourceLength, "Source length is 0");
#endif
        }
    };
} // namespace Ame::Rhi