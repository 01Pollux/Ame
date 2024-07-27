#pragma once

#include <exception>
#include <Asset/Core.hpp>

namespace Ame::Asset
{
    class AssetException : public std::runtime_error
    {
    public:
        AssetException(
            const UId&  uid,
            const char* message) noexcept :
            runtime_error(std::format("{} '{}'", message, UIdUtils::ToString(uid))),
            m_AssetGuid(uid)
        {
        }

        AssetException(
            const UId&  parentUId,
            const UId&  childUId,
            const char* message) noexcept :
            runtime_error(std::format("{} '{}::{}'", message, UIdUtils::ToString(parentUId), UIdUtils::ToString(childUId))),
            m_AssetGuid(parentUId)
        {
        }

        [[nodiscard]] const UId& GetAssetGuid() const noexcept
        {
            return m_AssetGuid;
        }

    private:
        const UId m_AssetGuid;
    };

    class InvalidAssetTypeException : public AssetException
    {
    public:
        InvalidAssetTypeException(
            const UId& uid) noexcept :
            AssetException(uid, "Invalid asset type")
        {
        }
    };

    class AssetNotFoundException : public AssetException
    {
    public:
        AssetNotFoundException(
            const UId& uid) noexcept :
            AssetException(uid, "Asset not found")
        {
        }

        AssetNotFoundException(
            const UId& parentUId,
            const UId& childUId) noexcept :
            AssetException(parentUId, childUId, "Asset not found in meta file")
        {
        }
    };

    class AssetWithNoHandlerException : public AssetException
    {
    public:
        AssetWithNoHandlerException(
            const UId& uid) noexcept :
            AssetException(uid, "Asset with no handler")
        {
        }

        AssetWithNoHandlerException(
            const UId& parentUId,
            const UId& childUId) noexcept :
            AssetException(parentUId, childUId, "Asset with no handler")
        {
        }
    };

    class AssetHandlerFailureException : public AssetException
    {
    public:
        AssetHandlerFailureException(
            const UId& uid) noexcept :
            AssetException(uid, "Asset handler failure")
        {
        }

        AssetHandlerFailureException(
            const UId& parentUId,
            const UId& childUId) noexcept :
            AssetException(parentUId, childUId, "Asset handler failure")
        {
        }
    };

    class AssetChildMetaNotFoundException : public AssetException
    {
    public:
        AssetChildMetaNotFoundException(
            const UId& parentUId,
            const UId& childUId) noexcept :
            AssetException(parentUId, childUId, "Asset child not found in meta file")
        {
        }
    };
} // namespace Ame::Asset
