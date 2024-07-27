#pragma once

#include <exception>
#include <Asset/Core.hpp>

namespace Ame::Asset
{
    class AssetException : public std::runtime_error
    {
    public:
        AssetException(
            const Guid& guid,
            const char* message) noexcept :
            runtime_error(std::format("{} '{}'", message, guid.ToString())),
            m_AssetGuid(guid)
        {
        }

        AssetException(
            const Guid& parentGuid,
            const Guid& childGuid,
            const char* message) noexcept :
            runtime_error(std::format("{} '{}::{}'", message, parentGuid.ToString(), childGuid.ToString())),
            m_AssetGuid(parentGuid)
        {
        }

        [[nodiscard]] const Guid& GetAssetGuid() const noexcept
        {
            return m_AssetGuid;
        }

    private:
        const Guid m_AssetGuid;
    };

    class InvalidAssetTypeException : public AssetException
    {
    public:
        InvalidAssetTypeException(
            const Guid& guid) noexcept :
            AssetException(guid, "Invalid asset type")
        {
        }
    };

    class AssetNotFoundException : public AssetException
    {
    public:
        AssetNotFoundException(
            const Guid& guid = Guid::c_Null) noexcept :
            AssetException(guid, "Asset not found")
        {
        }

        AssetNotFoundException(
            const Guid& parentGuid,
            const Guid& childGuid) noexcept :
            AssetException(parentGuid, childGuid, "Asset not found in meta file")
        {
        }
    };

    class AssetWithNoHandlerException : public AssetException
    {
    public:
        AssetWithNoHandlerException(
            const Guid& guid) noexcept :
            AssetException(guid, "Asset with no handler")
        {
        }

        AssetWithNoHandlerException(
            const Guid& parentGuid,
            const Guid& childGuid) noexcept :
            AssetException(parentGuid, childGuid, "Asset with no handler")
        {
        }
    };

    class AssetHandlerFailureException : public AssetException
    {
    public:
        AssetHandlerFailureException(
            const Guid& guid) noexcept :
            AssetException(guid, "Asset handler failure")
        {
        }

        AssetHandlerFailureException(
            const Guid& parentGuid,
            const Guid& childGuid) noexcept :
            AssetException(parentGuid, childGuid, "Asset handler failure")
        {
        }
    };

    class AssetChildMetaNotFoundException : public AssetException
    {
    public:
        AssetChildMetaNotFoundException(
            const Guid& parentGuid,
            const Guid& childGuid) noexcept :
            AssetException(parentGuid, childGuid, "Asset child not found in meta file")
        {
        }
    };
} // namespace Ame::Asset
