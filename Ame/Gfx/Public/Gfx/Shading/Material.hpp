#pragma once

#include <Gfx/Shading/Material.CommonState.hpp>
#include <Gfx/Shading/PropertyMap.hpp>

namespace Ame::Gfx::Shading
{
    class Material
    {
        static constexpr const char* UserDataPropertyTag = "";

    public:
        using PropertyHash     = uint64_t;
        using ResourceIterator = PropertyMap::ResourceMap::const_iterator;

    public:
        Material(
            Rhi::Device&              RhiDevice,
            Gfx::Cache::ShaderCache&  ShaderCache,
            Ptr<Rhi::PipelineLayout>  PipelineLayout,
            MaterialPipelineState     PipelineState,
            const PropertyDescriptor& Descriptor);

        Material(
            const Material* Mat);

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
            const String& Property,
            bool          Local = true);

    private:
        /// <summary>
        /// Check if the property is local to this material
        /// </summary>
        [[nodiscard]] bool IsLocal(
            const String& Property) const;

    public:
        template<typename Ty>
            requires Concepts::Gfx::Shading::ResourceMappable<Ty>
        void Set(
            const String& Property,
            const Ty&     Value)
        {
            SetScalar(Property, std::addressof(Value), ResourceMap<Ty>::Type, ResourceMap<Ty>::DataType);
        }

        void Set(
            const String&            Property,
            const Ptr<Rhi::Texture>& Texture,
            Rhi::TextureViewDesc     ViewDesc);

        void Set(
            const String&   Property,
            TextureResource Texture);

        void Set(
            const String&           Property,
            const Ptr<Rhi::Buffer>& Buffer,
            Rhi::BufferViewDesc     ViewDesc);

        void Set(
            const String&  Property,
            BufferResource Buffer);

        void Set(
            const String&    Property,
            Rhi::SamplerDesc SamplerDesc);

        void Set(
            const String&   Property,
            SamplerResource Sampler);

    public:
        template<typename Ty>
            requires Concepts::Gfx::Shading::ResourceMappable<Ty>
        [[nodiscard]] Ty Get(
            const String& Property) const
        {
            Ty Value{};
            GetScalar(Property, std::addressof(Value), ResourceMap<Ty>::Type, ResourceMap<Ty>::DataType);
            return Value;
        }

        [[nodiscard]] const TextureResource& GetTexture(
            const String& Property) const;

        [[nodiscard]] const BufferResource& GetBuffer(
            const String& Property) const;

        [[nodiscard]] const SamplerResource& GetSampler(
            const String& Property) const;

    public:
        /// <summary>
        /// Set the pipeline layout of this material
        /// </summary>
        [[nodiscard]] Ptr<Rhi::PipelineLayout> GetPipelineLayout() const;

        /// <summary>
        /// Set the pipeline state of this material
        /// </summary>
        [[nodiscard]] Co::result<Ptr<Rhi::PipelineState>> GetPipelineState(
            const MaterialRenderState& RenderState) const;

    public:
        /// <summary>
        /// Get the hash of the properties of this material
        /// </summary>
        [[nodiscard]] PropertyHash GetHash() const;

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
            const String& Property,
            const void*   Value,
            size_t        Size);

        void GetScalar(
            const String& Property,
            void*         Value,
            size_t        Size) const;

    private:
        void InvalidateHash();
        void UpdateHash() const;

    private:
        using PropertyLocalMap = boost::container::flat_map<String, bool>;

        struct LocalData
        {
            PropertyMap Properties;

            LocalData(
                const PropertyDescriptor& Descriptor);
            LocalData(
                const PropertyMap& Properties);
        };

        struct SharedData : LocalData
        {
            MaterialCommonState CommonState;

            SharedData(
                Rhi::Device&              RhiDevice,
                Gfx::Cache::ShaderCache&  ShaderCache,
                Ptr<Rhi::PipelineLayout>  PipelineLayout,
                MaterialPipelineState     PipelineState,
                const PropertyDescriptor& Descriptor);
        };

        Ptr<SharedData>  m_SharedData;
        LocalData        m_LocalData;
        PropertyLocalMap m_PropertyLocalMap;

        mutable Opt<PropertyHash> m_PropertiesHash;
    };
} // namespace Ame::Gfx::Shading