#pragma once

#include <Asset/Handle.hpp>
#include <variant>

namespace Ame::Asset
{
    class IAsset
    {
    public:
        IAsset(
            const Handle& AssetGuid,
            String        Path);

        virtual ~IAsset() = default;

        /// <summary>
        /// Gets the asset guid.
        /// </summary>
        [[nodiscard]] const Handle& GetGuid() const noexcept;

        /// <summary>
        /// Get the asset path.
        /// </summary>
        [[nodiscard]] const String& GetPath() const noexcept;

        /// <summary>
        /// Set the asset path.
        /// </summary>
        void SetPath(
            String Path) noexcept;

        /// <summary>
        /// Query if the asset is dirty.
        /// </summary>
        [[nodiscard]] bool IsDirty() const noexcept;

        /// <summary>
        /// Marks the asset as dirty.
        /// </summary>
        void MarkDirty(
            bool IsDirty = true) noexcept;

    protected:
        String       m_AssetPath;
        const Handle m_AssetGuid;
        bool         m_IsDirty = true;
    };

    using AssetPtr = Ptr<IAsset>;

    //

    template<typename Ty = IAsset>
        requires std::is_base_of_v<IAsset, Ty>
    class AssetTaskPtr
    {
    private:
        using AssetState = std::variant<
            std::monostate,
            Co::result<AssetPtr>,
            Ptr<Ty>>;

    public:
        static inline const Ptr<Ty> StaticNullPtr;

        AssetTaskPtr() :
            m_Asset(std::monostate{})
        {
        }

        AssetTaskPtr(
            Co::result<AssetPtr> Asset) :
            m_Asset(std::move(Asset))
        {
        }

        template<typename OTy = IAsset>
            requires std::is_base_of_v<IAsset, OTy>
        AssetTaskPtr(
            Ptr<OTy> Asset)
        {
            SetFromBase(std::move(Asset));
        }

        //

        void operator=(
            Co::result<AssetPtr> Asset) noexcept
        {
            m_Asset = std::move(Asset);
        }

        void operator=(
            Ptr<Ty> Asset) noexcept
        {
            m_Asset = std::move(Asset);
        }

        template<typename BaseTy>
            requires std::is_base_of_v<Ty, BaseTy>
        void operator=(
            AssetTaskPtr<BaseTy> Asset)
        {
            SetFromBase(std::move(Asset));
        }

        //

        [[nodiscard]] operator bool() const
        {
            return m_Asset.index() != 0;
        }

        [[nodiscard]] bool operator!() const
        {
            return !operator bool();
        }

        [[nodiscard]] auto Get()
        {
            Wait();
            return m_Asset.index() == 2 ? std::get<2>(m_Asset) : nullptr;
        }

        void Wait()
        {
            if (m_Asset.index() == 1) [[unlikely]]
            {
                SetFromBase(std::get<1>(m_Asset).get());
            }
        }

        [[nodiscard]] auto& operator->()
        {
            Wait();
            return std::get<2>(m_Asset);
        }

        [[nodiscard]] const Ptr<Ty>& operator*()
        {
            Wait();
            return std::get<2>(m_Asset);
        }

        [[nodiscard]] operator const Ptr<Ty>&()
        {
            Wait();
            return std::get<2>(m_Asset);
        }

        void Reset()
        {
            m_Asset = std::monostate{};
        }

    private:
        template<typename OTy>
        void SetFromBase(
            Ptr<OTy> Asset)
        {
            if constexpr (std::is_same_v<Ty, OTy>)
            {
                m_Asset = std::move(Asset);
            }
            else
            {
                m_Asset = std::dynamic_pointer_cast<Ty>(std::move(Asset));
            }
        }

    private:
        mutable AssetState m_Asset;
    };
} // namespace Ame::Asset