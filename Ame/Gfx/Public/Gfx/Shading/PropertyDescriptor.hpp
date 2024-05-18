#pragma once

#include <boost/property_tree/ptree.hpp>
#include <Gfx/Shading/Property.hpp>

namespace Ame::Gfx::Shading
{
    class PropertyDescriptor
    {
        friend class Material;

        struct PropertInfo
        {
            ResourceType     Type;
            ResourceDataType DataType;
            uint8_t          Dims;
        };

    public:
        static constexpr uint32_t UserDataAlignment = sizeof(float[4]);
        static constexpr uint32_t InvalidOffset     = std::numeric_limits<uint32_t>::max();

        using PropertyTree = boost::property_tree::ptree;

    public:
#define AME_DECLARE_MEMBER_SCALAR(FuncName, ImplName, Dims)                     \
    PropertyDescriptor& FuncName(const String& PropName, uint8_t ArraySize = 1) \
    {                                                                           \
        if (ArraySize > 1)                                                      \
        {                                                                       \
            for (uint8_t i = 0; i < ArraySize - 1; i++)                         \
            {                                                                   \
                PadToBoundaries();                                              \
                ImplName(std::format("{}[{}]", PropName, i), Dims);             \
                PadToBoundaries();                                              \
            }                                                                   \
            ImplName(std::format("{}[{}]", PropName, ArraySize - 1), Dims);     \
        }                                                                       \
        else                                                                    \
        {                                                                       \
            ImplName(PropName, Dims);                                           \
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

        AME_DECLARE_MEMBER_SCALAR(Matrix2x2, Matrix2x2Impl, 1);
        AME_DECLARE_MEMBER_SCALAR(Matrix3x3, Matrix3x3Impl, 1);
        AME_DECLARE_MEMBER_SCALAR(Matrix4x4, Matrix4x4Impl, 1);

        PropertyDescriptor& Struct(const String& PropName, const PropertyDescriptor& Descriptor, uint8_t ArraySize = 1)
        {
            if (ArraySize > 1)
            {
                for (uint8_t i = 0; i < ArraySize; ++i)
                {
                    StructImpl(std::format("{}[{}]", PropName, i), Descriptor);
                }
            }
            else
            {
                StructImpl(PropName, Descriptor);
            }
            return *this;
        }
        PropertyDescriptor& Resource(const String& PropName, ResourceType Type, ResourceDataType DataType);

#undef AME_DECLARE_MEMBER_SCALAR

    private:
        PropertyDescriptor& IntImpl(const String& PropName, uint8_t Dims);
        PropertyDescriptor& FloatImpl(const String& PropName, uint8_t Dims);
        PropertyDescriptor& BoolImpl(const String& PropName, uint8_t Dims);
        PropertyDescriptor& Matrix2x2Impl(const String& PropName, uint8_t Dims);
        PropertyDescriptor& Matrix3x3Impl(const String& PropName, uint8_t Dims);
        PropertyDescriptor& Matrix4x4Impl(const String& PropName, uint8_t Dims);
        PropertyDescriptor& StructImpl(const String& PropName, const PropertyDescriptor& Descriptor);

    public:
        [[nodiscard]] uint32_t GetStructSize() const;
        [[nodiscard]] uint32_t GetOffset(
            const String& PropName) const;

    private:
        [[nodiscard]] static uint32_t GetSize(
            ResourceType     Type,
            ResourceDataType DataType,
            uint8_t          Dims);

        [[nodiscard]] uint32_t AdvanceSize(
            uint32_t Size);

    private:
        void InsertProp(
            const String&      PropName,
            const PropertInfo& Info);

        void InsertStruct(
            const String&             PropName,
            const PropertyDescriptor& Descriptor);

        void TraverseAppendOffset(
            PropertyTree& Subtree,
            uint32_t      Offset);

        void InsertPadding(
            uint32_t Size);

        void PadToBoundaries();

    private:
        PropertyTree m_Tree;
        uint32_t     m_AlignedSize = 0;
    };
} // namespace Ame::Gfx::Shading