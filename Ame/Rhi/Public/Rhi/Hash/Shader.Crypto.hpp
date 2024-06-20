#pragma once

#include <Util/Crypto.hpp>
#include <Rhi/Shader/Shader.hpp>

namespace Ame::Util
{
    template<Concepts::CryptoAlgorithm CryptoAlgoTy>
    static void UpdateCrypto(
        CryptoAlgoTy&                 hasher,
        const Rhi::ShaderCompileDesc& desc)
    {
        for (auto& [key, value] : desc.Defines)
        {
            hasher.Update(std::bit_cast<const CryptoPP::byte*>(key.c_str()), sizeof(key[0]) * key.size());
            hasher.Update(std::bit_cast<const CryptoPP::byte*>(value.c_str()), sizeof(value[0]) * value.size());
        }
        for (auto extension : desc.SpirvExtensions)
        {
            hasher.Update(std::bit_cast<const CryptoPP::byte*>(&extension), sizeof(extension));
        }

        hasher.Update(std::bit_cast<const CryptoPP::byte*>(&desc.Stage), sizeof(desc.Stage));
        hasher.Update(std::bit_cast<const CryptoPP::byte*>(&desc.Profile), sizeof(desc.Profile));
        hasher.Update(std::bit_cast<const CryptoPP::byte*>(&desc.VulkanMemoryLayout), sizeof(desc.VulkanMemoryLayout));
        hasher.Update(std::bit_cast<const CryptoPP::byte*>(&desc.Flags), sizeof(desc.Flags));
    }

    template<Concepts::CryptoAlgorithm CryptoAlgoTy>
    static void UpdateCrypto(
        CryptoAlgoTy&              hasher,
        const Rhi::ShaderBytecode& shader)
    {
        hasher.Update(std::bit_cast<const CryptoPP::byte*>(shader.GetBytecode()), shader.GetSize());
    }
} // namespace Ame::Util