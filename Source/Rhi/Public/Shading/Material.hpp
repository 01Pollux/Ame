#pragma once

#include <Shading/Material.CommonState.hpp>
#include <Shading/Core.hpp>

namespace Ame::Rhi
{
    // {3D942721-30B7-4D6D-9453-731DB09F8259}
    static constexpr UId IID_Material{ 0x3d942721, 0x30b7, 0x4d6d, { 0x94, 0x53, 0x73, 0x1d, 0xb0, 0x9f, 0x82, 0x59 } };

    class Material : public BaseObject<IObject>
    {
    public:
        using Base = BaseObject<IObject>;

        IMPLEMENT_QUERY_INTERFACE_IN_PLACE(
            IID_Material, Base);

    public:
        [[nodiscard]] static Ptr<Material> Create(
            Dg::IRenderDevice* renderDevice,
            MaterialCreateDesc materialDesc);

    public:
        /// <summary>
        /// Clone new material from this material
        /// </summary>
        [[nodiscard]] Ptr<Material> Instantiate() const;

    public:
        /// <summary>
        /// Get the base material desc for the material
        /// </summary>
        [[nodiscard]] const MaterialShaderSourceStorage& GetMaterialShaders() const noexcept;

        /// <summary>
        /// Get the material hash for the base material desc
        /// </summary>
        [[nodiscard]] MaterialHash GetMaterialHash() const noexcept;

        /// <summary>
        /// Get the shader resource binding for the material
        /// </summary>
        [[nodiscard]] Dg::IShaderResourceBinding* GetBindings() const;

        /// <summary>
        /// Get the resource signature for the material
        /// </summary>
        [[nodiscard]] Dg::IPipelineResourceSignature* GetResourceSignature() const;

    public:
        void WriteUserData(
            const String&    propertyName,
            const std::byte* data,
            size_t           size);

        template<typename Ty>
            requires MaterialDataMappable<Ty>::Enabled
        void WriteUserData(
            const String& propertyName,
            const Ty&     data)
        {
            WriteUserData(propertyName, std::bit_cast<const std::byte*>(std::addressof(data)), sizeof(Ty));
        }

        void ReadUserData(
            const String& propertyName,
            std::byte*    data,
            size_t        size) const;

        template<typename Ty>
            requires MaterialDataMappable<Ty>::Enabled
        [[nodiscard]] Ty ReadUserData(
            const String& propertyName) const
        {
            Ty data;
            ReadUserData(propertyName, std::bit_cast<std::byte*>(std::addressof(data)), sizeof(Ty));
            return data;
        }

    public:
        [[nodiscard]] const std::byte* GetUserData() const;
        [[nodiscard]] uint32_t         GetSizeOfUserData() const;

    public:
        [[nodiscard]] const String& GetName() const;
        void                        SetName(const StringView& name);

    private:
        template<typename, typename>
        friend class Dg::MakeNewRCObj;

        Material(
            IReferenceCounters*       counters,
            Dg::IRenderDevice*        renderDevice,
            const MaterialCreateDesc& materialDesc);

        Material(
            IReferenceCounters* counters,
            const Material*     material);

    private:
        struct LocalData
        {
            String                          Name;
            UniquePtr<std::byte[]>          UserDataBuffer;
            Ptr<Dg::IShaderResourceBinding> Bindings;

            LocalData(
                const MaterialCommonState& commonState,
                const LocalData*           copyFrom = nullptr);

            LocalData(const LocalData&) = delete;
            LocalData(LocalData&&)      = delete;

            LocalData& operator=(const LocalData&) = delete;
            LocalData& operator=(LocalData&&)      = delete;

            ~LocalData() = default;
        };

        struct SharedData
        {
            Dg::IRenderDevice*  RenderDevice;
            MaterialCommonState CommonState;

            SharedData(
                Dg::IRenderDevice* renderDevice,
                MaterialCreateDesc createDesc);
        };

        SharedPtr<SharedData> m_SharedData;
        LocalData             m_LocalData;
    };
} // namespace Ame::Rhi