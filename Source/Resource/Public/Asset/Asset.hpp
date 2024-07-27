#pragma once

#include <variant>
#include <cereal/types/string.hpp>
#include <Asset/Core.hpp>

namespace Ame::Asset
{
    class IAsset : public ISerializable
    {
    public:
        void QueryInterface(
            const UId& uid,
            IObject**  object) override
        {
            if (uid == IID_BaseAsset)
            {
                *object = static_cast<IAsset*>(this);
                return;
            }
            ISerializable::QueryInterface(uid, object);
        }

    public:
        void Serialize(
            BinaryOArchiver& ar) const override
        {
            ar(m_AssetPath, m_AssetGuid);
        }

        void Deserialize(
            BinaryIArchiver& ar) override
        {
            ar(m_AssetPath, m_AssetGuid);
        }

    public:
        /// <summary>
        /// Gets the asset uid.
        /// </summary>
        [[nodiscard]] const UId& GetUId() const noexcept;

        /// <summary>
        /// Sets the asset uid.
        /// </summary>
        [[nodiscard]] void SetUId(
            const UId& uid) noexcept;

        /// <summary>
        /// Get the asset path.
        /// </summary>
        [[nodiscard]] const String& GetPath() const noexcept;

        /// <summary>
        /// Set the asset path.
        /// </summary>
        void SetPath(
            const String& path) noexcept;

        /// <summary>
        /// Query if the asset is dirty.
        /// </summary>
        [[nodiscard]] bool IsDirty() const noexcept;

        /// <summary>
        /// Marks the asset as dirty.
        /// </summary>
        void MarkDirty(
            bool isDirty) noexcept;

    protected:
        String m_AssetPath;
        UId    m_AssetGuid;
        bool   m_IsDirty = true;
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
        static inline const Ptr<Ty> c_NullPtr;

        AssetTaskPtr() :
            m_Asset(std::monostate{})
        {
        }

        AssetTaskPtr(
            Co::result<AssetPtr> asset) :
            m_Asset(std::move(asset))
        {
        }

        template<typename OTy = IAsset>
            requires std::is_base_of_v<IAsset, OTy>
        AssetTaskPtr(
            Ptr<OTy> asset)
        {
            SetFromBase(std::move(asset));
        }

        //

        void operator=(
            Co::result<AssetPtr> asset) noexcept
        {
            m_Asset = std::move(asset);
        }

        void operator=(
            Ptr<Ty> asset) noexcept
        {
            m_Asset = std::move(asset);
        }

        template<typename BaseTy>
            requires std::is_base_of_v<Ty, BaseTy>
        void operator=(
            AssetTaskPtr<BaseTy> asset)
        {
            SetFromBase(std::move(asset));
        }

        //

        [[nodiscard]] explicit operator bool() const noexcept
        {
            return m_Asset.index() != 0;
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
            Ptr<OTy> asset)
        {
            if constexpr (std::is_same_v<Ty, OTy>)
            {
                m_Asset = std::move(asset);
            }
            else
            {
                m_Asset = std::dynamic_pointer_cast<Ty>(std::move(asset));
            }
        }

    private:
        mutable AssetState m_Asset;
    };
} // namespace Ame::Asset