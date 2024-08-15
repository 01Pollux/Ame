#pragma once

#include <unordered_set>
#include <Rhi/Core.hpp>

namespace Ame::Rhi
{
#if PLATFORM_EMSCRIPTEN
    static constexpr char MultiDrawGLSLExtension[] = "#extension GL_ANGLE_multi_draw : enable";
#else
    static constexpr char MultiDrawGLSLExtension[] = "#extension GL_ARB_shader_draw_parameters : enable";
#endif

    class ShaderCreateInfoX
    {
    public:
        ShaderCreateInfoX() = default;
        ShaderCreateInfoX(
            const Dg::ShaderCreateInfo& createInfo)
        {
            CopyFrom(createInfo);
        }

        ShaderCreateInfoX(
            const ShaderCreateInfoX& other)
        {
            CopyFrom(other.GetCreateInfo());
        }

        ShaderCreateInfoX& operator=(
            const ShaderCreateInfoX& other)
        {
            if (this != &other)
            {
                CopyFrom(other.GetCreateInfo());
            }
            return *this;
        }

        ShaderCreateInfoX(
            ShaderCreateInfoX&& other) = default;

        ShaderCreateInfoX& operator=(
            ShaderCreateInfoX&& other) = default;

        ~ShaderCreateInfoX() = default;

    private:
        void CopyFrom(
            const Dg::ShaderCreateInfo& createInfo)
        {
            this->FilePathCStr(createInfo.FilePath)
                .SourceInputStreamFactory(Ptr{ createInfo.pShaderSourceStreamFactory })
                .SourceCode(createInfo.Source == nullptr ? "" : String{ createInfo.Source, createInfo.SourceLength })
                .Macros({ createInfo.Macros.Elements, createInfo.Macros.Elements + createInfo.Macros.Count })
                .ShaderType(createInfo.Desc.ShaderType)
                .CombinedTextureSamplers(createInfo.Desc.UseCombinedTextureSamplers)
                .CombinedSamplerSuffix(createInfo.Desc.CombinedSamplerSuffix)
                .SourceLanguage(createInfo.SourceLanguage)
                .ShaderCompiler(createInfo.ShaderCompiler)
                .HLSLVersion(createInfo.HLSLVersion)
                .GLSLVersion(createInfo.GLSLVersion)
                .GLESSLVersion(createInfo.GLESSLVersion)
                .MSLVersion(createInfo.MSLVersion)
                .CompileFlags(createInfo.CompileFlags)
                .LoadConstantBufferReflection(createInfo.LoadConstantBufferReflection)
                .GLSLExtensionsCStr(createInfo.GLSLExtensions)
                .WebGPUEmulatedArrayIndexSuffixCStr(createInfo.WebGPUEmulatedArrayIndexSuffix);
            if (createInfo.Source)
            {
                SourceCodeCStr(createInfo.Source, createInfo.SourceLength);
            }
            else if (createInfo.ByteCode)
            {
                ByteCode(createInfo.ByteCode, createInfo.ByteCodeSize);
            }
        }

    public:
        ShaderCreateInfoX&
        NameCStr(
            const char* name)
        {
            m_CreateInfo.Desc.Name = name == nullptr ? "" : m_StringPool.insert(std::move(name)).first->c_str();
            return *this;
        }

        ShaderCreateInfoX& Name(
            String name)
        {
            m_CreateInfo.Desc.Name = m_StringPool.insert(std::move(name)).first->c_str();
            return *this;
        }

        [[nodiscard]] const char* Name() const noexcept
        {
            return m_CreateInfo.Desc.Name;
        }

    public:
        ShaderCreateInfoX& FilePathCStr(
            const char* path)
        {
            m_CreateInfo.FilePath = path == nullptr ? "" : m_StringPool.insert(std::move(path)).first->c_str();
            return *this;
        }

        ShaderCreateInfoX& FilePath(
            String path)
        {
            m_CreateInfo.FilePath = m_StringPool.insert(std::move(path)).first->c_str();
            return *this;
        }

        [[nodiscard]] const char* FilePath() const noexcept
        {
            return m_CreateInfo.FilePath;
        }

        //

        ShaderCreateInfoX& SourceInputStreamFactory(
            Ptr<Dg::IShaderSourceInputStreamFactory> factory)
        {
            m_CreateInfo.pShaderSourceStreamFactory = factory;
            m_ShaderSourceStreamFactory             = std::move(factory);
            return *this;
        }

        [[nodiscard]] Dg::IShaderSourceInputStreamFactory* SourceInputStreamFactory() const noexcept
        {
            return m_CreateInfo.pShaderSourceStreamFactory;
        }

        //

        ShaderCreateInfoX& SourceCodeCStr(
            const char* sourceCode,
            size_t      length = 0)
        {
            m_CreateInfo.Source       = sourceCode == nullptr ? "" : m_StringPool.insert(std::move(sourceCode)).first->c_str();
            m_CreateInfo.SourceLength = length;
            return *this;
        }

        ShaderCreateInfoX& SourceCode(
            String sourceCode)
        {
            m_CreateInfo.SourceLength = sourceCode.size();
            m_CreateInfo.Source       = m_StringPool.insert(std::move(sourceCode)).first->c_str();
            return *this;
        }

        [[nodiscard]] const char* SourceCode() const noexcept
        {
            return m_CreateInfo.Source;
        }

        [[nodiscard]] size_t SourceCodeLength() const noexcept
        {
            return m_CreateInfo.SourceLength;
        }

        //

        ShaderCreateInfoX& ByteCode(
            std::unique_ptr<std::byte[]> byteCode,
            size_t                       size)
        {
            m_ByteCode                = std::move(byteCode);
            m_CreateInfo.ByteCodeSize = size;
            return *this;
        }

        ShaderCreateInfoX& ByteCode(
            const void* byteCode,
            size_t      size)
        {
            m_ByteCode.release();
            if (size)
            {
                m_ByteCode = std::make_unique<std::byte[]>(size);
                std::ranges::copy_n(static_cast<const std::byte*>(byteCode), size, m_ByteCode.get());
            }
            m_CreateInfo.ByteCodeSize = size;
            return *this;
        }

        [[nodiscard]] const std::byte* ByteCode() const noexcept
        {
            return m_ByteCode.get();
        }

        [[nodiscard]] size_t ByteCodeSize() const noexcept
        {
            return m_CreateInfo.ByteCodeSize;
        }

        //

        ShaderCreateInfoX& EntryPoint(
            String entryPoint)
        {
            m_CreateInfo.EntryPoint = m_StringPool.insert(std::move(entryPoint)).first->c_str();
            return *this;
        }

        [[nodiscard]] const char* EntryPoint() const noexcept
        {
            return m_CreateInfo.EntryPoint;
        }

        //

        ShaderCreateInfoX& Macros(
            std::vector<Dg::ShaderMacro> macros)
        {
            m_Macros = std::move(macros);
            for (auto& macro : m_Macros)
            {
                macro.Name       = m_StringPool.insert(macro.Name).first->c_str();
                macro.Definition = m_StringPool.insert(macro.Definition).first->c_str();
            }
            m_CreateInfo.Macros = { m_Macros.data(), Count32(m_Macros) };
            return *this;
        }

        ShaderCreateInfoX& AppendMacro(
            Dg::ShaderMacro macro)
        {
            m_Macros.emplace_back(m_StringPool.insert(macro.Name).first->c_str(), m_StringPool.insert(macro.Definition).first->c_str());
            m_CreateInfo.Macros = { m_Macros.data(), Count32(m_Macros) };
            return *this;
        }

        ShaderCreateInfoX& AppendMacros(
            std::span<const Dg::ShaderMacro> macros)
        {
            m_Macros.reserve(m_Macros.size() + macros.size());
            for (auto& macro : macros)
            {
                m_Macros.emplace_back(m_StringPool.insert(macro.Name).first->c_str(), m_StringPool.insert(macro.Definition).first->c_str());
            }
            m_CreateInfo.Macros = { m_Macros.data(), Count32(m_Macros) };
            return *this;
        }

        [[nodiscard]] const std::vector<Dg::ShaderMacro>& Macros() const noexcept
        {
            return m_Macros;
        }

        void ClearMacros() noexcept
        {
            m_Macros.clear();
            m_CreateInfo.Macros = { nullptr, 0 };
        }

        //

        ShaderCreateInfoX& ShaderType(
            Dg::SHADER_TYPE shaderType)
        {
            m_CreateInfo.Desc.ShaderType = shaderType;
            return *this;
        }

        [[nodiscard]] Dg::SHADER_TYPE ShaderType() const noexcept
        {
            return m_CreateInfo.Desc.ShaderType;
        }

        //

        ShaderCreateInfoX& CombinedTextureSamplers(
            bool combinedTextureSamplers)
        {
            m_CreateInfo.Desc.UseCombinedTextureSamplers = combinedTextureSamplers;
            return *this;
        }

        [[nodiscard]] bool CombinedTextureSamplers() const noexcept
        {
            return m_CreateInfo.Desc.UseCombinedTextureSamplers;
        }

        //

        ShaderCreateInfoX& CombinedSamplerSuffix(
            String combinedSamplerSuffix)
        {
            m_CreateInfo.Desc.CombinedSamplerSuffix = m_StringPool.insert(std::move(combinedSamplerSuffix)).first->c_str();
            return *this;
        }

        [[nodiscard]] const char* CombinedSamplerSuffix() const noexcept
        {
            return m_CreateInfo.Desc.CombinedSamplerSuffix;
        }

        //

        ShaderCreateInfoX& SourceLanguage(
            Dg::SHADER_SOURCE_LANGUAGE sourceLanguage)
        {
            m_CreateInfo.SourceLanguage = sourceLanguage;
            return *this;
        }

        [[nodiscard]] Dg::SHADER_SOURCE_LANGUAGE SourceLanguage() const noexcept
        {
            return m_CreateInfo.SourceLanguage;
        }

        //

        ShaderCreateInfoX& ShaderCompiler(
            Dg::SHADER_COMPILER shaderCompiler)
        {
            m_CreateInfo.ShaderCompiler = shaderCompiler;
            return *this;
        }

        [[nodiscard]] Dg::SHADER_COMPILER ShaderCompiler() const noexcept
        {
            return m_CreateInfo.ShaderCompiler;
        }

        //

        ShaderCreateInfoX& HLSLVersion(
            Dg::ShaderVersion shaderVersion)
        {
            m_CreateInfo.HLSLVersion = shaderVersion;
            return *this;
        }

        [[nodiscard]] Dg::ShaderVersion HLSLVersion() const noexcept
        {
            return m_CreateInfo.HLSLVersion;
        }

        //

        ShaderCreateInfoX& GLSLVersion(
            Dg::ShaderVersion shaderVersion)
        {
            m_CreateInfo.GLSLVersion = shaderVersion;
            return *this;
        }

        [[nodiscard]] Dg::ShaderVersion GLSLVersion() const noexcept
        {
            return m_CreateInfo.GLSLVersion;
        }

        //

        ShaderCreateInfoX& GLESSLVersion(
            Dg::ShaderVersion shaderVersion)
        {
            m_CreateInfo.GLESSLVersion = shaderVersion;
            return *this;
        }

        [[nodiscard]] Dg::ShaderVersion GLESSLVersion() const noexcept
        {
            return m_CreateInfo.GLESSLVersion;
        }

        //

        ShaderCreateInfoX& MSLVersion(
            Dg::ShaderVersion shaderVersion)
        {
            m_CreateInfo.MSLVersion = shaderVersion;
            return *this;
        }

        [[nodiscard]] Dg::ShaderVersion MSLVersion() const noexcept
        {
            return m_CreateInfo.MSLVersion;
        }

        //

        ShaderCreateInfoX& CompileFlags(
            Dg::SHADER_COMPILE_FLAGS compileFlags)
        {
            m_CreateInfo.CompileFlags = compileFlags;
            return *this;
        }

        [[nodiscard]] Dg::SHADER_COMPILE_FLAGS CompileFlags() const noexcept
        {
            return m_CreateInfo.CompileFlags;
        }

        //

        ShaderCreateInfoX& LoadConstantBufferReflection(
            bool loadConstantBufferReflection)
        {
            m_CreateInfo.LoadConstantBufferReflection = loadConstantBufferReflection;
            return *this;
        }

        [[nodiscard]] bool LoadConstantBufferReflection() const noexcept
        {
            return m_CreateInfo.LoadConstantBufferReflection;
        }

        //

        ShaderCreateInfoX& GLSLExtensionsCStr(
            const char* path)
        {
            m_CreateInfo.GLSLExtensions = path == nullptr ? "" : m_StringPool.insert(std::move(path)).first->c_str();
            return *this;
        }

        ShaderCreateInfoX& GLSLExtensions(
            String glslExtensions)
        {
            m_CreateInfo.GLSLExtensions = m_StringPool.insert(std::move(glslExtensions)).first->c_str();
            return *this;
        }

        ShaderCreateInfoX& AppendGLSLExtensions(
            String glslExtensions)
        {
            return GLSLExtensions(std::format("{}\n{}", (m_CreateInfo.GLSLExtensions ? m_CreateInfo.GLSLExtensions : ""), "\n", glslExtensions));
        }

        [[nodiscard]] const char* GLSLExtensions() const noexcept
        {
            return m_CreateInfo.GLSLExtensions;
        }

        //

        ShaderCreateInfoX& WebGPUEmulatedArrayIndexSuffixCStr(
            const char* webGPUEmulatedArrayIndexSuffix)
        {
            m_CreateInfo.WebGPUEmulatedArrayIndexSuffix = webGPUEmulatedArrayIndexSuffix == nullptr ? "" : m_StringPool.insert(std::move(webGPUEmulatedArrayIndexSuffix)).first->c_str();
            return *this;
        }

        ShaderCreateInfoX& WebGPUEmulatedArrayIndexSuffix(
            String webGPUEmulatedArrayIndexSuffix)
        {
            m_CreateInfo.WebGPUEmulatedArrayIndexSuffix = m_StringPool.insert(std::move(webGPUEmulatedArrayIndexSuffix)).first->c_str();
            return *this;
        }

        [[nodiscard]] const char* WebGPUEmulatedArrayIndexSuffix() const noexcept
        {
            return m_CreateInfo.WebGPUEmulatedArrayIndexSuffix;
        }

        //

        [[nodiscard]] const Dg::ShaderCreateInfo& GetCreateInfo() const noexcept
        {
            return m_CreateInfo;
        }

        [[nodiscard]] explicit operator const Dg::ShaderCreateInfo&() const noexcept
        {
            return m_CreateInfo;
        }

    private:
        Ptr<Dg::IShaderSourceInputStreamFactory> m_ShaderSourceStreamFactory;
        Dg::ShaderCreateInfo                     m_CreateInfo;
        std::unique_ptr<std::byte[]>             m_ByteCode;
        std::unordered_set<std::string>          m_StringPool;
        std::vector<Dg::ShaderMacro>             m_Macros;
    };
} // namespace Ame::Rhi