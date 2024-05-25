#include <stack>
#include <Math/Common.hpp>

#include <Gfx/Shading/PropertyDescriptor.hpp>

#include <Log/Wrapper.hpp>

#define AME_PROPDESC_METANAME     "meta"
#define AME_PROPDESC_METADIMS     "dims"
#define AME_PROPDESC_METATYPE     "type"
#define AME_PROPDESC_METADATATYPE "data_type"
#define AME_PROPDESC_METAOFFSET   "offset"
#define AME_PROPDESC_META(Name)   AME_PROPDESC_METANAME##"."##Name

namespace Ame::Gfx::Shading
{
    PropertyDescriptor& PropertyDescriptor::IntImpl(
        const String&    propertyName,
        Rhi::ShaderFlags flags,
        uint8_t          dims)
    {
        InsertProp(propertyName, { ResourceType::Scalar, ResourceDataType::Int, dims }, flags);
        return *this;
    }

    PropertyDescriptor& PropertyDescriptor::FloatImpl(
        const String&    propertyName,
        Rhi::ShaderFlags flags,
        uint8_t          dims)
    {
        InsertProp(propertyName, { ResourceType::Scalar, ResourceDataType::Float, dims }, flags);
        return *this;
    }

    PropertyDescriptor& PropertyDescriptor::BoolImpl(
        const String&    propertyName,
        Rhi::ShaderFlags flags,
        uint8_t          dims)
    {
        InsertProp(propertyName, { ResourceType::Scalar, ResourceDataType::Bool, dims }, flags);
        return *this;
    }

    PropertyDescriptor& PropertyDescriptor::Matrix2x2Impl(
        const String&    propertyName,
        Rhi::ShaderFlags flags,
        uint8_t          dims)
    {
        InsertProp(propertyName, { ResourceType::Scalar, ResourceDataType::Matrix2x2, dims }, flags);
        return *this;
    }

    PropertyDescriptor& PropertyDescriptor::Matrix3x3Impl(
        const String&    propertyName,
        Rhi::ShaderFlags flags,
        uint8_t          dims)
    {
        InsertProp(propertyName, { ResourceType::Scalar, ResourceDataType::Matrix3x3, dims }, flags);
        return *this;
    }

    PropertyDescriptor& PropertyDescriptor::Matrix4x4Impl(
        const String&    propertyName,
        Rhi::ShaderFlags flags,
        uint8_t          dims)
    {
        InsertProp(propertyName, { ResourceType::Scalar, ResourceDataType::Matrix4x4, dims }, flags);
        return *this;
    }

    PropertyDescriptor& PropertyDescriptor::StructImpl(
        const String&             propertyName,
        const PropertyDescriptor& descriptor,
        Rhi::ShaderFlags          flags)
    {
        InsertStruct(propertyName, descriptor, flags);
        return *this;
    }

    PropertyDescriptor& PropertyDescriptor::Resource(
        const String&    propertyName,
        ResourceType     type,
        ResourceDataType dataType,
        Rhi::ShaderFlags flags)
    {
        m_Resources.emplace_back(ResourceInfo{ { type, dataType, 1 }, flags, propertyName });
        return *this;
    }

    //

    uint32_t PropertyDescriptor::GetStructSize() const
    {
        return m_AlignedSize;
    }

    Rhi::ShaderFlags PropertyDescriptor::GetStructAccessFlags() const
    {
        return m_ShaderBits;
    }

    uint32_t PropertyDescriptor::GetOffset(
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

    uint32_t PropertyDescriptor::GetResourceCount() const
    {
        return static_cast<uint32_t>(m_Resources.size());
    }

    auto PropertyDescriptor::GetResources() const -> Co::generator<CRef<ResourceInfo>>
    {
        for (auto& property : m_Resources)
        {
            co_yield property;
        }
    }

    //

    bool PropertyDescriptor::IsEmpty() const
    {
        return m_Tree.empty();
    }

    //

    uint32_t PropertyDescriptor::GetSize(
        ResourceType     type,
        ResourceDataType dataType,
        uint8_t          dims)
    {
        size_t size = 0;
        if (type == ResourceType::Scalar)
        {
            switch (dataType)
            {
            case ResourceDataType::Int:
                size = sizeof(int);
                break;
            case ResourceDataType::Float:
                size = sizeof(float);
                break;
            case ResourceDataType::Bool:
                size = sizeof(int);
                break;
            case ResourceDataType::Matrix2x2:
                size = sizeof(float[2][2]);
                break;
            case ResourceDataType::Matrix3x3:
                size = sizeof(float[3][3]);
                break;
            case ResourceDataType::Matrix4x4:
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

    uint32_t PropertyDescriptor::AdvanceSize(
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

    void PropertyDescriptor::InsertProp(
        const String&       propertyName,
        const PropertyInfo& propertyInfo,
        Rhi::ShaderFlags    flags)
    {
        uint32_t size   = GetSize(propertyInfo.Type, propertyInfo.DataType, propertyInfo.Dims);
        uint32_t offset = AdvanceSize(size);

        auto& property = m_Tree.put_child(propertyName, {});
        auto& metaData = property.put_child(AME_PROPDESC_METANAME, {});

        metaData.put(AME_PROPDESC_METADIMS, propertyInfo.Dims);
        metaData.put(AME_PROPDESC_METATYPE, std::to_underlying(propertyInfo.Type));
        metaData.put(AME_PROPDESC_METADATATYPE, std::to_underlying(propertyInfo.DataType));
        metaData.put(AME_PROPDESC_METAOFFSET, offset);

        m_ShaderBits.Set(flags);
    }

    void PropertyDescriptor::InsertStruct(
        const String&             propertyName,
        const PropertyDescriptor& descriptor,
        Rhi::ShaderFlags          flags)
    {
#ifdef AME_DEBUG
        Log::Gfx().Assert(descriptor.GetResourceCount() == 0, "Structs cannot contain resources");
#endif

        PadToBoundaries();

        uint32_t size   = descriptor.GetStructSize();
        uint32_t offset = AdvanceSize(size);

        auto& property = m_Tree.put_child(propertyName, descriptor.m_Tree);
        auto& metaData = property.put_child(AME_PROPDESC_METANAME, {});

        metaData.put(AME_PROPDESC_METADIMS, 1);
        metaData.put(AME_PROPDESC_METATYPE, std::to_underlying(ResourceType::Struct));
        metaData.put(AME_PROPDESC_METADATATYPE, std::to_underlying(ResourceDataType::None));
        metaData.put(AME_PROPDESC_METAOFFSET, offset);

        TraverseAppendOffset(property, offset);
        m_ShaderBits.Set(flags);
    }

    //

    void PropertyDescriptor::TraverseAppendOffset(
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
            auto  type     = static_cast<ResourceType>(metaData.get<std::underlying_type_t<ResourceType>>(AME_PROPDESC_METATYPE));

            metaData.put(AME_PROPDESC_METAOFFSET, metaData.get<uint32_t>(AME_PROPDESC_METAOFFSET) + offset);

            if (type == ResourceType::Struct)
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

    void PropertyDescriptor::InsertPadding(
        uint32_t size)
    {
        m_AlignedSize += size;
    }

    void PropertyDescriptor::PadToBoundaries()
    {
        m_AlignedSize = Math::AlignUp(m_AlignedSize, UserDataAlignment);
    }
} // namespace Ame::Gfx::Shading
