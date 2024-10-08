#pragma once

#include <istream>
#include <string>
#include <array>

#include <cryptopp/cryptlib.h>

#include <Core/String.hpp>

namespace Ame::Util
{
    template<typename T>
    concept CryptoAlgorithmType = requires {
        { std::is_base_of_v<CryptoPP::HashTransformation, T> } -> std::convertible_to<bool>;
        { T::DIGESTSIZE } -> std::convertible_to<size_t>;
    };

    template<CryptoAlgorithmType CryptoAlgoTy>
    using CryptoDigest = std::array<CryptoPP::byte, CryptoAlgoTy::DIGESTSIZE>;

    /// <summary>
    /// Helper function to update crypto from trivial type
    /// </summary>
    template<CryptoAlgorithmType CryptoAlgoTy, typename Ty>
        requires std::is_standard_layout_v<Ty> && std::is_trivial_v<Ty>
    void UpdateCrypto(CryptoAlgoTy& hasher, const Ty& value)
    {
        hasher.Update(std::bit_cast<const CryptoPP::byte*>(&value), sizeof(Ty));
    }

    /// <summary>
    /// Helper function to update crypto from stream
    /// </summary>
    template<CryptoAlgorithmType CryptoAlgoTy>
    void UpdateCrypto(CryptoAlgoTy& hasher, std::istream& stream, size_t size)
    {
        CryptoPP::byte buffer[64];
        while (size > 0 && stream.good())
        {
            size_t readSize = std::min(size, std::size(buffer));
            stream.read(std::bit_cast<char*>(&buffer[0]), readSize);
            hasher.Update(buffer, readSize);
            size -= readSize;
        }
    }

    /// <summary>
    /// Helper function to convert digest to string
    /// </summary>
    template<CryptoAlgorithmType CryptoAlgoTy>
    [[nodiscard]] inline String DigestStringify(const CryptoDigest<CryptoAlgoTy>& digest)
    {
        constexpr const char lut[] = "0123456789ABCDEF";

        String output;
        output.reserve(2 * std::size(digest));
        for (CryptoPP::byte c : digest)
        {
            output.push_back(lut[c >> 4]);
            output.push_back(lut[c & 15]);
        }

        return output;
    }

    /// <summary>
    /// Helper function to finalize digest
    /// </summary>
    template<CryptoAlgorithmType CryptoAlgoTy> [[nodiscard]] auto FinalizeDigest(CryptoAlgoTy& hasher)
    {
        CryptoDigest<CryptoAlgoTy> digest;
        hasher.Final(digest.data());
        return digest;
    }

    /// <summary>
    /// Helper function to finalize digest to string
    /// </summary>
    template<CryptoAlgorithmType CryptoAlgoTy> [[nodiscard]] String FinalizeDigestToString(CryptoAlgoTy& hasher)
    {
        CryptoDigest<CryptoAlgoTy> digest;
        hasher.Final(digest.data());
        return DigestStringify<CryptoAlgoTy>(digest);
    }
} // namespace Ame::Util