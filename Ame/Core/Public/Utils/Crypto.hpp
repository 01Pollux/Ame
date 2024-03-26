#pragma once

#include <cryptopp/cryptlib.h>

#include <istream>
#include <string>
#include <array>

namespace Ame::Concepts
{
    template<typename T>
    concept CryptoAlgorithm = requires {
        {
            std::is_base_of_v<CryptoPP::HashTransformation, T>
        } -> std::convertible_to<bool>;
        {
            T::DIGESTSIZE
        } -> std::convertible_to<size_t>;
    };
} // namespace Ame::Concepts

namespace Ame::Utils
{
    template<Concepts::CryptoAlgorithm CryptoAlgoTy>
    using CryptoDigest = std::array<CryptoPP::byte, CryptoAlgoTy::DIGESTSIZE>;

    /// <summary>
    /// Helper function to update crypto from trivial type
    /// </summary>
    template<Concepts::CryptoAlgorithm CryptoAlgoTy, typename Ty>
        requires std::is_trivially_copyable_v<Ty>
    static void UpdateCrypto(
        CryptoAlgoTy& Hasher,
        const Ty&     Value)
    {
        Hasher.Update(std::bit_cast<const CryptoPP::byte*>(&Value), sizeof(Ty));
    }

    /// <summary>
    /// Helper function to update crypto from stream
    /// </summary>
    template<Concepts::CryptoAlgorithm CryptoAlgoTy>
    static void UpdateCrypto(
        CryptoAlgoTy& Hasher,
        std::istream& Stream,
        size_t        Size)
    {
        CryptoPP::byte Bytes[64];
        while (Size > 0 && Stream.good())
        {
            size_t ReadSize = std::min(Size, std::size(Bytes));
            Stream.read(std::bit_cast<char*>(&Bytes[0]), ReadSize);
            Hasher.Update(Bytes, ReadSize);
            Size -= ReadSize;
        }
    }

    /// <summary>
    /// Helper function to convert digest to string
    /// </summary>
    template<Concepts::CryptoAlgorithm CryptoAlgoTy>
    [[nodiscard]] static std::string DigestStringify(
        const CryptoDigest<CryptoAlgoTy>& Digest)
    {
        constexpr const char Lut[] = "0123456789ABCDEF";

        std::string Output;
        Output.reserve(2 * std::size(Digest));
        for (CryptoPP::byte c : Digest)
        {
            Output.push_back(Lut[c >> 4]);
            Output.push_back(Lut[c & 15]);
        }

        return Output;
    }

    /// <summary>
    /// Helper function to finalize digest
    /// </summary>
    template<Concepts::CryptoAlgorithm CryptoAlgoTy>
    [[nodiscard]] static auto FinalizeDigest(
        CryptoAlgoTy& Hasher)
    {
        CryptoDigest<CryptoAlgoTy> Digest;
        Hasher.Final(Digest.data());
        return Digest;
    }

    /// <summary>
    /// Helper function to finalize digest to string
    /// </summary>
    template<Concepts::CryptoAlgorithm CryptoAlgoTy>
    [[nodiscard]] static std::string FinalizeDigestToString(
        CryptoAlgoTy& Hasher)
    {
        CryptoDigest<CryptoAlgoTy> Digest;
        Hasher.Final(Digest.data());
        return DigestStringify<CryptoAlgoTy>(Digest);
    }
} // namespace Ame::Utils