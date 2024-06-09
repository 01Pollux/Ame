#pragma once

#include <Gfx/Shading/Material.CommonState.hpp>
#include <Gfx/Shading/PropertyMap.hpp>

namespace Ame::Gfx::Shading
{
    class Material
    {
        static constexpr const char* UserDataPropertyTag = "";

    public:
        using PropertyHash      = uint64_t;
        using PipelineStateHash = MaterialCommonState::PipelineStateHash;
        using ResourceIterator  = PropertyMap::ResourceMap::const_iterator;

    public:
        Material(
            Rhi::Device&              rhiDevice,
            Gfx::Cache::ShaderCache&  shaderCache,
            Ptr<Rhi::PipelineLayout>  pipelineLayout,
            MaterialPipelineState     pipelineState,
            const PropertyDescriptor& descriptor);

        Material(
            const Material* material);

    public:
        /// <summary>
        /// Clone new material from this material
        /// </summary>
        [[nodiscard]] Ptr<Material> Instantiate() const;

    public:
        /// <summary>
        /// Make a property local to this material
        /// A local property is a property that is not shared between master and instance materials
        /// If the property is nested inside a struct, the struct will be made local
        /// </summary>
        void MakeLocal(
            const String& propertyName,
            bool          local = true);

    private:
        /// <summary>
        /// Check if the property is local to this material
        /// </summary>
        [[nodiscard]] bool IsLocal(
            const String& propertyName) const;

    public:
        template<typename Ty>
            requires Gfx::Shading::ResourceMappable<Ty>::Enabled
        void Set(
            const String& propertyName,
            const Ty&     value)
        {
            SetScalar(propertyName, std::bit_cast<const std::byte*>(std::addressof(value)), sizeof(value));
        }

        void Set(
            const String&            propertyName,
            const Ptr<Rhi::Texture>& texture,
            Rhi::TextureViewDesc     desc);

        void Set(
            const String&   propertyName,
            TextureResource textureResource);

        void Set(
            const String&           propertyName,
            const Ptr<Rhi::Buffer>& buffer,
            Rhi::BufferViewDesc     desc);

        void Set(
            const String&  propertyName,
            BufferResource bufferResource);

        void Set(
            const String&    propertyName,
            Rhi::SamplerDesc desc);

        void Set(
            const String&   propertyName,
            SamplerResource samplerResource);

    public:
        template<typename Ty>
            requires Gfx::Shading::ResourceMappable<Ty>::Enabled
        [[nodiscard]] Ty Get(
            const String& propertyName) const
        {
            Ty value{};
            GetScalar(propertyName, std::bit_cast<std::byte*>(std::addressof(value)), sizeof(Ty));
            return value;
        }

        [[nodiscard]] const TextureResource& GetTexture(
            const String& propertyName) const;

        [[nodiscard]] const BufferResource& GetBuffer(
            const String& propertyName) const;

        [[nodiscard]] const SamplerResource& GetSampler(
            const String& propertyName) const;

    public:
        /// <summary>
        /// Set the pipeline layout of this material
        /// </summary>
        [[nodiscard]] Ptr<Rhi::PipelineLayout> GetPipelineLayout() const;

        /// <summary>
        /// Set the pipeline state of this material
        /// </summary>
        [[nodiscard]] Co::result<Ptr<Rhi::PipelineState>> GetPipelineState(
            const MaterialRenderState& renderState) const;

    public:
        /// <summary>
        /// Get the hash of the properties of this material
        /// </summary>
        [[nodiscard]] PropertyHash GetPropertyHash() const;

        /// <summary>
        /// Get the hash of the pipelinestate of this material
        /// </summary>
        [[nodiscard]] const PipelineStateHash& GetPipelineHash() const;

    public:
        /// <summary>
        /// Get the size of the user data of this material
        /// </summary>
        [[nodiscard]] uint32_t GetSizeOfUserData() const;

        /// <summary>
        /// Get the user data of this material
        /// </summary>
        [[nodiscard]] const void* GetUserData() const;

        /// <summary>
        /// Get all resources of this material
        /// </summary>
        [[nodiscard]] Co::generator<ResourceIterator> GetResources() const;

    private:
        void SetScalar(
            const String&    property,
            const std::byte* data,
            size_t           size);

        void GetScalar(
            const String& property,
            std::byte*    data,
            size_t        size) const;

    private:
        void InvalidatePropertyHash();
        void UpdatePropertyHash() const;

    private:
        using PropertyLocalMap = boost::container::flat_map<String, bool>;

        struct LocalData
        {
            PropertyMap Properties;

            LocalData(
                const PropertyDescriptor& descriptor);
            LocalData(
                const PropertyMap& properties);
        };

        struct SharedData : LocalData
        {
            MaterialCommonState CommonState;

            SharedData(
                Rhi::Device&              rhiDevice,
                Gfx::Cache::ShaderCache&  shaderCache,
                Ptr<Rhi::PipelineLayout>  pipelineLayout,
                MaterialPipelineState     pipelineState,
                const PropertyDescriptor& descriptor);
        };

        Ptr<SharedData>  m_SharedData;
        LocalData        m_LocalData;
        PropertyLocalMap m_PropertyLocalMap;

        mutable Opt<PropertyHash> m_PropertiesHash;
    };
} // namespace Ame::Gfx::Shading