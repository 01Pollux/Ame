#pragma once

#include <Util/Crypto.hpp>
#include <Rhi/Resource/Shader.hpp>
#include <Rhi/Resource/Shader.Compiler.hpp>

namespace Ame::Util
{
    template<Concepts::CryptoAlgorithm CryptoAlgoTy>
    static void UpdateCrypto(
        CryptoAlgoTy&                 Hasher,
        const Rhi::ShaderCompileDesc& Desc)
    {
        for (auto& [Key, Value] : Desc.Defines)
        {
            Hasher.Update(std::bit_cast<const CryptoPP::byte*>(Key.c_str()), sizeof(Key[0]) * Key.size());
            Hasher.Update(std::bit_cast<const CryptoPP::byte*>(Value.c_str()), sizeof(Value[0]) * Value.size());
        }
        for (auto Extension : Desc.SpirvExtensions)
        {
            Hasher.Update(std::bit_cast<const CryptoPP::byte*>(&Extension), sizeof(Extension));
        }

        auto Stage = Desc.GetStage();
        Hasher.Update(std::bit_cast<const CryptoPP::byte*>(&Stage), sizeof(Stage));
        Hasher.Update(std::bit_cast<const CryptoPP::byte*>(&Desc.Profile), sizeof(Desc.Profile));
        Hasher.Update(std::bit_cast<const CryptoPP::byte*>(&Desc.VulkanMemoryLayout), sizeof(Desc.VulkanMemoryLayout));
        Hasher.Update(std::bit_cast<const CryptoPP::byte*>(&Desc.Flags), sizeof(Desc.Flags));
    }

    template<Concepts::CryptoAlgorithm CryptoAlgoTy>
    static void UpdateCrypto(
        CryptoAlgoTy&              Hasher,
        const Rhi::ShaderBytecode& Bytecode)
    {
        Hasher.Update(Bytecode.GetBytecode(), Bytecode.GetSize());
    }
} // namespace Ame::Util