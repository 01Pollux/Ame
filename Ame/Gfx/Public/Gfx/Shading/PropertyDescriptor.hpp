#pragma once

#include <boost/property_tree/ptree.hpp>
#include <Gfx/Shading/Property.hpp>

namespace Ame::Gfx::Shading
{
    class PropertyDescriptor
    {
        friend class Material;

    public:
        struct PropertyInfo
        {
            ResourceType     Type;
            ResourceDataType DataType;
            uint8_t          Dims;
        };

        struct ResourceInfo : PropertyInfo
        {
            Rhi::ShaderFlags ShaderFlags;
            String           PropName;
        };

        static constexpr uint32_t UserDataAlignment = sizeof(float[4]);
        static constexpr uint32_t InvalidOffset     = std::numeric_limits<uint32_t>::max();

        using PropertyTree = boost::property_tree::ptree;

    public:
#define AME_DECLARE_MEMBER_SCALAR(FuncName, ImplName, Dims)                            \
    PropertyDescriptor& FuncName(                                                      \
        const String&    propertyName,                                                 \
        Rhi::ShaderFlags flags     = Rhi::ShaderFlags::Graphics(),                     \
        uint8_t          arraySize = 1)                                                \
    {                                                                                  \
        if (arraySize > 1)                                                             \
        {                                                                              \
            for (uint8_t i = 0; i < arraySize - 1; i++)                                \
            {                                                                          \
                PadToBoundaries();                                                     \
                ImplName(std::format("{}[{}]", propertyName, i), flags, Dims);         \
                PadToBoundaries();                                                     \
            }                                                                          \
            ImplName(std::format("{}[{}]", propertyName, arraySize - 1), flags, Dims); \
        }                                                                              \
        else                                                                           \
        {                                                                              \
            ImplName(propertyName, flags, Dims);                                       \
        }                                                                              \
        return *this;                                                                  \
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

        PropertyDescriptor& Struct(
            const String&             propertyName,
            const PropertyDescriptor& descriptor,
            Rhi::ShaderFlags          flags     = Rhi::ShaderFlags::Graphics(),
            uint8_t                   arraySize = 1)
        {
            if (arraySize > 1)
            {
                for (uint8_t i = 0; i < arraySize; i++)
                {
                    StructImpl(std::format("{}[{}]", propertyName, i), descriptor, flags);
                }
            }
            else
            {
                StructImpl(propertyName, descriptor, flags);
            }
            return *this;
        }

        PropertyDescriptor& Resource(
            const String&    propertyName,
            ResourceType     type,
            ResourceDataType dataType,
            Rhi::ShaderFlags flags = Rhi::ShaderFlags::Graphics());

        PropertyDescriptor& Sampler(
            const String&    propertyName,
            Rhi::ShaderFlags flags = Rhi::ShaderFlags::Graphics());

#undef AME_DECLARE_MEMBER_SCALAR

    private:
        PropertyDescriptor& IntImpl(const String& propertyName, Rhi::ShaderFlags flags, uint8_t dims);
        PropertyDescriptor& FloatImpl(const String& propertyName, Rhi::ShaderFlags flags, uint8_t dims);
        PropertyDescriptor& BoolImpl(const String& propertyName, Rhi::ShaderFlags flags, uint8_t dims);
        PropertyDescriptor& Matrix2x2Impl(const String& propertyName, Rhi::ShaderFlags flags, uint8_t dims);
        PropertyDescriptor& Matrix3x3Impl(const String& propertyName, Rhi::ShaderFlags flags, uint8_t dims);
        PropertyDescriptor& Matrix4x4Impl(const String& propertyName, Rhi::ShaderFlags flags, uint8_t dims);
        PropertyDescriptor& StructImpl(const String& propertyName, const PropertyDescriptor& descriptor, Rhi::ShaderFlags flags);

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
            const String& propertyName) const;

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
            ResourceType     type,
            ResourceDataType dataType,
            uint8_t          dims);

        /// <summary>
        /// Increase the size of the descriptor userdata and return offset for the next property
        /// </summary>
        [[nodiscard]] uint32_t AdvanceSize(
            uint32_t size);

    private:
        void InsertProp(
            const String&       propertyName,
            const PropertyInfo& propertyInfo,
            Rhi::ShaderFlags    flags);

        void InsertStruct(
            const String&             propertyName,
            const PropertyDescriptor& descriptor,
            Rhi::ShaderFlags          flags);

    private:
        void TraverseAppendOffset(
            PropertyTree& subtree,
            uint32_t      offset);

        void InsertPadding(
            uint32_t size);

        void PadToBoundaries();

    private:
        PropertyTree              m_Tree;
        std::vector<ResourceInfo> m_Resources;

        uint32_t         m_AlignedSize = 0;
        Rhi::ShaderFlags m_ShaderBits;
    };
} // namespace Ame::Gfx::Shading