#pragma once

#include <boost/property_tree/ptree.hpp>
#include <Shading/Property.hpp>

namespace Ame::Rhi
{
    class MaterialPropertyDescriptor
    {
        friend class Material;

    public:
        struct PropertyInfo
        {
            MaterialResourceType Type;
            MaterialDataType     DataType;
            uint8_t              Dims;
        };

        static constexpr uint32_t UserDataAlignment = sizeof(float[4]);
        static constexpr uint32_t InvalidOffset     = std::numeric_limits<uint32_t>::max();

        using PropertyTree = boost::property_tree::ptree;

    public:
#define AME_DECLARE_MEMBER_SCALAR(FuncName, ImplName, Dims)                     \
    MaterialPropertyDescriptor& FuncName(                                       \
        const String& propertyName,                                             \
        uint8_t       arraySize = 1)                                            \
    {                                                                           \
        if (arraySize > 1)                                                      \
        {                                                                       \
            for (uint8_t i = 0; i < arraySize - 1; i++)                         \
            {                                                                   \
                PadToBoundaries();                                              \
                ImplName(std::format("{}[{}]", propertyName, i), Dims);         \
                PadToBoundaries();                                              \
            }                                                                   \
            ImplName(std::format("{}[{}]", propertyName, arraySize - 1), Dims); \
        }                                                                       \
        else                                                                    \
        {                                                                       \
            ImplName(propertyName, Dims);                                       \
        }                                                                       \
        return *this;                                                           \
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

        AME_DECLARE_MEMBER_SCALAR(Matrix3x3, Matrix3x3Impl, 1);
        AME_DECLARE_MEMBER_SCALAR(Matrix4x4, Matrix4x4Impl, 1);

        MaterialPropertyDescriptor& Struct(
            const String&                     propertyName,
            const MaterialPropertyDescriptor& descriptor,
            uint8_t                           arraySize = 1)
        {
            if (arraySize > 1)
            {
                for (uint8_t i = 0; i < arraySize; i++)
                {
                    StructImpl(std::format("{}[{}]", propertyName, i), descriptor);
                }
            }
            else
            {
                StructImpl(propertyName, descriptor);
            }
            return *this;
        }

#undef AME_DECLARE_MEMBER_SCALAR

    private:
        MaterialPropertyDescriptor& IntImpl(const String& propertyName, uint8_t dims);
        MaterialPropertyDescriptor& FloatImpl(const String& propertyName, uint8_t dims);
        MaterialPropertyDescriptor& BoolImpl(const String& propertyName, uint8_t dims);
        MaterialPropertyDescriptor& Matrix3x3Impl(const String& propertyName, uint8_t dims);
        MaterialPropertyDescriptor& Matrix4x4Impl(const String& propertyName, uint8_t dims);
        MaterialPropertyDescriptor& StructImpl(const String& propertyName, const MaterialPropertyDescriptor& descriptor);

    public:
        /// <summary>
        /// Get the size of the descriptor in bytes excluding resource properties
        /// </summary>
        [[nodiscard]] uint32_t GetStructSize() const;

        /// <summary>
        /// Get offset of a property in the descriptor for user data, returns InvalidOffset if the property is not found or if its a resource
        /// </summary>
        [[nodiscard]] uint32_t GetOffset(
            const String& propertyName) const;

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
            MaterialResourceType type,
            MaterialDataType     dataType,
            uint8_t              dims);

        /// <summary>
        /// Increase the size of the descriptor userdata and return offset for the next property
        /// </summary>
        [[nodiscard]] uint32_t AdvanceSize(
            uint32_t size);

    private:
        void InsertProp(
            const String&       propertyName,
            const PropertyInfo& propertyInfo);

        void InsertStruct(
            const String&                     propertyName,
            const MaterialPropertyDescriptor& descriptor);

    private:
        void TraverseAppendOffset(
            PropertyTree& subtree,
            uint32_t      offset);

        void InsertPadding(
            uint32_t size);

        void PadToBoundaries();

    private:
        PropertyTree m_Tree;
        uint32_t     m_AlignedSize = 0;
    };
} // namespace Ame::Rhi