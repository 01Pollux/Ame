#pragma once

#include <boost/property_tree/ptree.hpp>
#include <Gfx/Shading/Property.hpp>

namespace Ame::Gfx::Shading
{
    class PropertyDescriptor
    {
        friend class Material;

    public:
        struct PropertInfo
        {
            ResourceType     Type;
            ResourceDataType DataType;
            uint8_t          Dims;
        };

        struct ResourceInfo : PropertInfo
        {
            Rhi::ShaderFlags ShaderFlags;
            String           PropName;
        };

        static constexpr uint32_t UserDataAlignment = sizeof(float[4]);
        static constexpr uint32_t InvalidOffset     = std::numeric_limits<uint32_t>::max();

        using PropertyTree = boost::property_tree::ptree;

    public:
#define AME_DECLARE_MEMBER_SCALAR(FuncName, ImplName, Dims)                                                                             \
    PropertyDescriptor& FuncName(const String& PropName, Rhi::ShaderFlags ShaderFlags = Rhi::ShaderFlags::All(), uint8_t ArraySize = 1) \
    {                                                                                                                                   \
        if (ArraySize > 1)                                                                                                              \
        {                                                                                                                               \
            for (uint8_t i = 0; i < ArraySize - 1; i++)                                                                                 \
            {                                                                                                                           \
                PadToBoundaries();                                                                                                      \
                ImplName(std::format("{}[{}]", PropName, i), ShaderFlags, Dims);                                                        \
                PadToBoundaries();                                                                                                      \
            }                                                                                                                           \
            ImplName(std::format("{}[{}]", PropName, ArraySize - 1), ShaderFlags, Dims);                                                \
        }                                                                                                                               \
        else                                                                                                                            \
        {                                                                                                                               \
            ImplName(PropName, ShaderFlags, Dims);                                                                                      \
        }                                                                                                                               \
        return *this;                                                                                                                   \
    }

        AME_DECLARE_MEMBER_SCALAR(Int, IntImpl, 1);
        AME_DECLARE_MEMBER_SCALAR(Int2, IntImpl, 2);
        AME_DECLARE_MEMBER_SCALAR(Int3, IntImpl, 3);
        AME_DECLARE_MEMBER_SCALAR(Int4, IntImpl, 4);

        AME_DECLARE_MEMBER_SCALAR(Float, FloatImpl, 1);
        AME_DECLARE_MEMBER_SCALAR(Float2, FloatImpl, 2);
        AME_DECLARE_MEMBER_SCALAR(Float3, FloatImpl, 3);
        AME_DECLARE_MEMBER_SCALAR(Float4, FloatImpl, 4);

        AME_DECLARE_MEMBER_SCALAR(Bool, BoolImpl, 1);
        AME_DECLARE_MEMBER_SCALAR(Bool2, BoolImpl, 2);
        AME_DECLARE_MEMBER_SCALAR(Bool3, BoolImpl, 3);
        AME_DECLARE_MEMBER_SCALAR(Bool4, BoolImpl, 4);

        AME_DECLARE_MEMBER_SCALAR(Matrix2x2, Matrix2x2Impl, 1);
        AME_DECLARE_MEMBER_SCALAR(Matrix3x3, Matrix3x3Impl, 1);
        AME_DECLARE_MEMBER_SCALAR(Matrix4x4, Matrix4x4Impl, 1);

        PropertyDescriptor& Struct(const String& PropName, const PropertyDescriptor& Descriptor, Rhi::ShaderFlags Flags = Rhi::ShaderFlags::All(), uint8_t ArraySize = 1)
        {
            if (ArraySize > 1)
            {
                for (uint8_t i = 0; i < ArraySize; ++i)
                {
                    StructImpl(std::format("{}[{}]", PropName, i), Descriptor, Flags);
                }
            }
            else
            {
                StructImpl(PropName, Descriptor, Flags);
            }
            return *this;
        }
        PropertyDescriptor& Resource(const String& PropName, ResourceType Type, ResourceDataType DataType, Rhi::ShaderFlags Flags = Rhi::ShaderFlags::All());

#undef AME_DECLARE_MEMBER_SCALAR

    private:
        PropertyDescriptor& IntImpl(const String& PropName, Rhi::ShaderFlags Flags, uint8_t Dims);
        PropertyDescriptor& FloatImpl(const String& PropName, Rhi::ShaderFlags Flags, uint8_t Dims);
        PropertyDescriptor& BoolImpl(const String& PropName, Rhi::ShaderFlags Flags, uint8_t Dims);
        PropertyDescriptor& Matrix2x2Impl(const String& PropName, Rhi::ShaderFlags Flags, uint8_t Dims);
        PropertyDescriptor& Matrix3x3Impl(const String& PropName, Rhi::ShaderFlags Flags, uint8_t Dims);
        PropertyDescriptor& Matrix4x4Impl(const String& PropName, Rhi::ShaderFlags Flags, uint8_t Dims);
        PropertyDescriptor& StructImpl(const String& PropName, const PropertyDescriptor& Descriptor, Rhi::ShaderFlags Flags);

    public:
        /// <summary>
        /// Get the size of the descriptor in bytes excluding resource properties
        /// </summary>
        [[nodiscard]] uint32_t GetStructSize() const;

        /// <summary>
        /// Get the access flags for the descriptor for user data
        /// </summary>
        [[nodiscard]] Rhi::ShaderFlags GetStructAccessFlags() const;

        /// <summary>
        /// Get offset of a property in the descriptor for user data, returns InvalidOffset if the property is not found or if its a resource
        /// </summary>
        [[nodiscard]] uint32_t GetOffset(
            const String& PropName) const;

    public:
        /// <summary>
        /// Get number of resources in the descriptor
        /// </summary>
        [[nodiscard]] uint32_t GetResourceCount() const;

        /// <summary>
        /// Get the resources in the descriptor
        /// </summary>
        [[nodiscard]] Co::generator<CRef<ResourceInfo>> GetResources() const;

    public:
        /// <summary>
        /// Check if the descriptor is empty (no userdata or resources)
        /// </summary>
        [[nodiscard]] bool IsEmpty() const;

    private:
        /// <summary>
        /// Get size of datatype and dimensions
        /// </summary>
        [[nodiscard]] static uint32_t GetSize(
            ResourceType     Type,
            ResourceDataType DataType,
            uint8_t          Dims);

        /// <summary>
        /// Increase the size of the descriptor userdata and return offset for the next property
        /// </summary>
        [[nodiscard]] uint32_t AdvanceSize(
            uint32_t Size);

    private:
        void InsertProp(
            const String&      PropName,
            const PropertInfo& Info,
            Rhi::ShaderFlags   Flags);

        void InsertStruct(
            const String&             PropName,
            const PropertyDescriptor& Descriptor,
            Rhi::ShaderFlags          Flags);

    private:
        void TraverseAppendOffset(
            PropertyTree& Subtree,
            uint32_t      Offset);

        void InsertPadding(
            uint32_t Size);

        void PadToBoundaries();

    private:
        PropertyTree              m_Tree;
        std::vector<ResourceInfo> m_Resources;

        uint32_t         m_AlignedSize = 0;
        Rhi::ShaderFlags m_ShaderBits;
    };
} // namespace Ame::Gfx::Shading