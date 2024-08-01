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
        static constexpr const char* UserDataPropertyTag = "UserDataBuffer";

    public:
        [[nodiscard]] static Ptr<Material> Create(
            Dg::IRenderDevice*        renderDevice,
            const MaterialCreateDesc& materialDesc);

    public:
        /// <summary>
        /// Clone new material from this material
        /// </summary>
        [[nodiscard]] Ptr<Material> Instantiate() const;

    public:
        /// <summary>
        /// Get the base material desc for the material
        /// </summary>
        [[nodiscard]] const MaterialDesc& GetMaterialDesc() const noexcept;

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
        /// <summary>
        /// Get the size of the user data of this material, 0 if there is no user data
        /// </summary>
        [[nodiscard]] uint32_t GetSizeOfUserData() const;

        /// <summary>
        /// Beging mapping of user data
        /// </summary>
        [[nodiscard]] std::byte* MapUserData(
            Dg::IDeviceContext* deviceContext,
            Dg::MAP_FLAGS       mapFlags = Dg::MAP_FLAG_NO_OVERWRITE);

        /// <summary>
        /// Beging mapping of user data
        /// </summary>
        template<typename Ty>
        [[nodiscard]] Ty* MapUserData(
            Dg::IDeviceContext* deviceContext,
            Dg::MAP_FLAGS       mapFlags = Dg::MAP_FLAG_NO_OVERWRITE)
        {
            return std::bit_cast<Ty*>(MapUserData(deviceContext, mapFlags));
        }

        /// <summary>
        /// End mapping of user data
        /// </summary>
        void UnmapUserData(
            Dg::IDeviceContext* deviceContext);

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
            Ptr<Dg::IBuffer>                UserDataBuffer;
            Ptr<Dg::IShaderResourceBinding> Bindings;

            LocalData(
                Dg::IRenderDevice*         renderDevice,
                const MaterialCommonState& commonState);

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
                Dg::IRenderDevice*        renderDevice,
                const MaterialCreateDesc& createDesc);
        };

        SharedPtr<SharedData> m_SharedData;
        LocalData             m_LocalData;
    };
} // namespace Ame::Rhi