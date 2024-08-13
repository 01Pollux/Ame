#include <stack>
#include <Math/Common.hpp>

#include <Shading/PropertyDescriptor.hpp>

#include <Log/Wrapper.hpp>

#define AME_PROPDESC_METANAME     "meta"
#define AME_PROPDESC_METADIMS     "dims"
#define AME_PROPDESC_METATYPE     "type"
#define AME_PROPDESC_METADATATYPE "data_type"
#define AME_PROPDESC_METAOFFSET   "offset"
#define AME_PROPDESC_META(Name)   AME_PROPDESC_METANAME##"."##Name

namespace Ame::Rhi
{
    MaterialPropertyDescriptor& MaterialPropertyDescriptor::IntImpl(
        const String& propertyName,
        uint8_t       dims)
    {
        InsertProp(propertyName, { MaterialResourceType::Scalar, MaterialDataType::Int, dims });
        return *this;
    }

    MaterialPropertyDescriptor& MaterialPropertyDescriptor::FloatImpl(
        const String& propertyName,
        uint8_t       dims)
    {
        InsertProp(propertyName, { MaterialResourceType::Scalar, MaterialDataType::Float, dims });
        return *this;
    }

    MaterialPropertyDescriptor& MaterialPropertyDescriptor::BoolImpl(
        const String& propertyName,
        uint8_t       dims)
    {
        InsertProp(propertyName, { MaterialResourceType::Scalar, MaterialDataType::Bool, dims });
        return *this;
    }

    MaterialPropertyDescriptor& MaterialPropertyDescriptor::Matrix3x3Impl(
        const String& propertyName,
        uint8_t       dims)
    {
        InsertProp(propertyName, { MaterialResourceType::Scalar, MaterialDataType::Matrix3x3, dims });
        return *this;
    }

    MaterialPropertyDescriptor& MaterialPropertyDescriptor::Matrix4x4Impl(
        const String& propertyName,
        uint8_t       dims)
    {
        InsertProp(propertyName, { MaterialResourceType::Scalar, MaterialDataType::Matrix4x4, dims });
        return *this;
    }

    MaterialPropertyDescriptor& MaterialPropertyDescriptor::StructImpl(
        const String&                     propertyName,
        const MaterialPropertyDescriptor& descriptor)
    {
        InsertStruct(propertyName, descriptor);
        return *this;
    }

    //

    uint32_t MaterialPropertyDescriptor::GetStructSize() const
    {
        return m_AlignedSize;
    }

    uint32_t MaterialPropertyDescriptor::GetOffset(
        const String& propertyName) const
    {
        return m_Tree.get_child_optional(propertyName)
            .map([](const boost::property_tree::ptree& property)
                 { return property.get_optional<uint32_t>(
                                      AME_PROPDESC_META(AME_PROPDESC_METAOFFSET))
                       .value_or(InvalidOffset); })
            .value_or(InvalidOffset);
    }

    //

    bool MaterialPropertyDescriptor::IsEmpty() const
    {
        return m_Tree.empty();
    }

    //

    uint32_t MaterialPropertyDescriptor::GetSize(
        MaterialResourceType type,
        MaterialDataType     dataType,
        uint8_t              dims)
    {
        size_t size = 0;
        if (type == MaterialResourceType::Scalar)
        {
            switch (dataType)
            {
            case MaterialDataType::Bool:
            case MaterialDataType::Int:
            case MaterialDataType::UInt:
                size = sizeof(int);
                break;
            case MaterialDataType::Bool2:
            case MaterialDataType::Int2:
            case MaterialDataType::UInt2:
                size = sizeof(int[2]);
                break;
            case MaterialDataType::Bool3:
            case MaterialDataType::Int3:
            case MaterialDataType::UInt3:
                size = sizeof(int[3]);
                break;
            case MaterialDataType::Bool4:
            case MaterialDataType::Int4:
            case MaterialDataType::UInt4:
                size = sizeof(int[4]);
                break;

            case MaterialDataType::Float:
                size = sizeof(float);
                break;
            case MaterialDataType::Float2:
                size = sizeof(float[2]);
                break;
            case MaterialDataType::Float3:
                size = sizeof(float[3]);
                break;
            case MaterialDataType::Float4:
                size = sizeof(float[4]);
                break;

            case MaterialDataType::Matrix3x3:
                size = sizeof(float[3][3]);
                break;
            case MaterialDataType::Matrix4x4:
                size = sizeof(float[4][4]);
                break;
            }

            if (dims > 1)
            {
                size = size * dims;
            }
        }
        return size;
    }

    uint32_t MaterialPropertyDescriptor::AdvanceSize(
        uint32_t size)
    {
        uint32_t offset = InvalidOffset;

        if (size)
        {
            // if offset + size cant fit in 'UserDataAlignment' bytes, move to next 16 bytes boundary
            if (((m_AlignedSize % UserDataAlignment) + size > UserDataAlignment))
            {
                m_AlignedSize = Math::AlignUp(m_AlignedSize, UserDataAlignment);
            }
            offset = m_AlignedSize;
            m_AlignedSize += size;
        }

        return offset;
    }

    //

    void MaterialPropertyDescriptor::InsertProp(
        const String&       propertyName,
        const PropertyInfo& propertyInfo)
    {
        uint32_t size   = GetSize(propertyInfo.Type, propertyInfo.DataType, propertyInfo.Dims);
        uint32_t offset = AdvanceSize(size);

        auto& property = m_Tree.put_child(propertyName, {});
        auto& metaData = property.put_child(AME_PROPDESC_METANAME, {});

        metaData.put(AME_PROPDESC_METADIMS, propertyInfo.Dims);
        metaData.put(AME_PROPDESC_METATYPE, std::to_underlying(propertyInfo.Type));
        metaData.put(AME_PROPDESC_METADATATYPE, std::to_underlying(propertyInfo.DataType));
        metaData.put(AME_PROPDESC_METAOFFSET, offset);
    }

    void MaterialPropertyDescriptor::InsertStruct(
        const String&                     propertyName,
        const MaterialPropertyDescriptor& descriptor)
    {
        PadToBoundaries();

        uint32_t size   = descriptor.GetStructSize();
        uint32_t offset = AdvanceSize(size);

        auto& property = m_Tree.put_child(propertyName, descriptor.m_Tree);
        auto& metaData = property.put_child(AME_PROPDESC_METANAME, {});

        metaData.put(AME_PROPDESC_METADIMS, 1);
        metaData.put(AME_PROPDESC_METATYPE, std::to_underlying(MaterialResourceType::Struct));
        metaData.put(AME_PROPDESC_METADATATYPE, std::to_underlying(MaterialDataType::None));
        metaData.put(AME_PROPDESC_METAOFFSET, offset);

        TraverseAppendOffset(property, offset);
    }

    //

    void MaterialPropertyDescriptor::TraverseAppendOffset(
        PropertyTree& subtree,
        uint32_t      offset)
    {
        std::stack<Ref<PropertyTree>> trees;

        for (auto& [name, child] : subtree)
        {
            if (name != AME_PROPDESC_METANAME)
            {
                trees.push(child);
            }
        }

        while (!trees.empty())
        {
            auto& child = trees.top().get();
            trees.pop();

            auto& metaData = child.get_child(AME_PROPDESC_METANAME);
            auto  type     = static_cast<MaterialResourceType>(metaData.get<std::underlying_type_t<MaterialResourceType>>(AME_PROPDESC_METATYPE));

            metaData.put(AME_PROPDESC_METAOFFSET, metaData.get<uint32_t>(AME_PROPDESC_METAOFFSET) + offset);

            if (type == MaterialResourceType::Struct)
            {
                for (auto& [name, subChild] : child)
                {
                    if (name != AME_PROPDESC_METANAME)
                    {
                        trees.push(subChild);
                    }
                }
            }
        }
    }

    void MaterialPropertyDescriptor::InsertPadding(
        uint32_t size)
    {
        m_AlignedSize += size;
    }

    void MaterialPropertyDescriptor::PadToBoundaries()
    {
        m_AlignedSize = Math::AlignUp(m_AlignedSize, UserDataAlignment);
    }
} // namespace Ame::Rhi