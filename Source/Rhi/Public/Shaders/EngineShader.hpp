#pragma once

#include <vector>
#include <boost/iostreams/filter/gzip.hpp>
#include <boost/iostreams/device/array.hpp>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/iostreams/copy.hpp>
#include <Rhi/Core.hpp>

namespace Ame::Rhi
{
    class EngineShader
    {
    public:
        void AddMacro(
            const char* name,
            const char* definition)
        {
            m_Macros.emplace_back(Dg::ShaderMacro{ name, definition });
        }

        operator const Dg::ShaderCreateInfo&() const
        {
            m_CreateInfo.Macros = { m_Macros.data(), static_cast<uint32_t>(m_Macros.size()) };
            return m_CreateInfo;
        }

        void SetFactory(
            Dg::IShaderSourceInputStreamFactory* factory)
        {
            m_CreateInfo.pShaderSourceStreamFactory = factory;
        }

        [[nodiscard]] const Dg::ShaderCreateInfo& GetCreateInfo() const
        {
            return m_CreateInfo;
        }

    protected:
        void Setup(
            const Dg::ShaderDesc&      desc,
            StringView                 sourceCode,
            Dg::SHADER_SOURCE_LANGUAGE lang = Dg::SHADER_SOURCE_LANGUAGE_HLSL)
        {
            m_CreateInfo.SourceLanguage = lang;
            m_CreateInfo.Desc           = desc;
            m_CreateInfo.Source         = sourceCode.data();
            m_CreateInfo.SourceLength   = sourceCode.size() - 1;
        }

        void SetupCompressed(
            const Dg::ShaderDesc&      desc,
            std::span<const uint8_t>   compressedCode,
            Dg::SHADER_SOURCE_LANGUAGE lang = Dg::SHADER_SOURCE_LANGUAGE_HLSL)
        {
            namespace bio = boost::iostreams;

            bio::filtering_istream stream;
            stream.push(bio::gzip_decompressor());
            stream.push(bio::array_source(std::bit_cast<const char*>(compressedCode.data()), compressedCode.size()));
            bio::copy(stream, std::back_inserter(m_DecompressedCode));

            m_CreateInfo.SourceLanguage = lang;
            m_CreateInfo.Desc           = desc;
            m_CreateInfo.Source         = m_DecompressedCode.data();
            m_CreateInfo.SourceLength   = m_DecompressedCode.size();
        }

    protected:
        mutable Dg::ShaderCreateInfo m_CreateInfo;
        std::vector<Dg::ShaderMacro> m_Macros;
        std::vector<char>            m_DecompressedCode;
    };
} // namespace Ame::Rhi