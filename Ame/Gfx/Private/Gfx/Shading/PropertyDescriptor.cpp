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
        const String&    PropName,
        Rhi::ShaderFlags Flags,
        uint8_t          Dims)
    {
        InsertProp(PropName, { ResourceType::Scalar, ResourceDataType::Int, Dims }, Flags);
        return *this;
    }

    PropertyDescriptor& PropertyDescriptor::FloatImpl(
        const String&    PropName,
        Rhi::ShaderFlags Flags,
        uint8_t          Dims)
    {
        InsertProp(PropName, { ResourceType::Scalar, ResourceDataType::Float, Dims }, Flags);
        return *this;
    }

    PropertyDescriptor& PropertyDescriptor::BoolImpl(
        const String&    PropName,
        Rhi::ShaderFlags Flags,
        uint8_t          Dims)
    {
        InsertProp(PropName, { ResourceType::Scalar, ResourceDataType::Bool, Dims }, Flags);
        return *this;
    }

    PropertyDescriptor& PropertyDescriptor::Matrix2x2Impl(
        const String&    PropName,
        Rhi::ShaderFlags Flags,
        uint8_t          Dims)
    {
        InsertProp(PropName, { ResourceType::Scalar, ResourceDataType::Matrix2x2, Dims }, Flags);
        return *this;
    }

    PropertyDescriptor& PropertyDescriptor::Matrix3x3Impl(
        const String&    PropName,
        Rhi::ShaderFlags Flags,
        uint8_t          Dims)
    {
        InsertProp(PropName, { ResourceType::Scalar, ResourceDataType::Matrix3x3, Dims }, Flags);
        return *this;
    }

    PropertyDescriptor& PropertyDescriptor::Matrix4x4Impl(
        const String&    PropName,
        Rhi::ShaderFlags Flags,
        uint8_t          Dims)
    {
        InsertProp(PropName, { ResourceType::Scalar, ResourceDataType::Matrix4x4, Dims }, Flags);
        return *this;
    }

    PropertyDescriptor& PropertyDescriptor::StructImpl(
        const String&             PropName,
        const PropertyDescriptor& Descriptor,
        Rhi::ShaderFlags          Flags)
    {
        InsertStruct(PropName, Descriptor, Flags);
        return *this;
    }

    PropertyDescriptor& PropertyDescriptor::Resource(
        const String&    PropName,
        ResourceType     Type,
        ResourceDataType DataType,
        Rhi::ShaderFlags Flags)
    {
        m_Resources.emplace_back(ResourceInfo{ { Type, DataType, 1 }, Flags, PropName });
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
        const String& PropName) const
    {
        return m_Tree.get_child_optional(PropName)
            .map([](const boost::property_tree::ptree& Prop)
                 { return Prop.get_optional<uint32_t>(
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
        for (auto& Prop : m_Resources)
        {
            co_yield Prop;
        }
    }

    //

    bool PropertyDescriptor::IsEmpty() const
    {
        return m_Tree.empty();
    }

    //

    uint32_t PropertyDescriptor::GetSize(
        ResourceType     Type,
        ResourceDataType DataType,
        uint8_t          Dims)
    {
        size_t Size = 0;
        if (Type == ResourceType::Scalar)
        {
            switch (DataType)
            {
            case ResourceDataType::Int:
                Size = sizeof(int);
                break;
            case ResourceDataType::Float:
                Size = sizeof(float);
                break;
            case ResourceDataType::Bool:
                Size = sizeof(int);
                break;
            case ResourceDataType::Matrix2x2:
                Size = sizeof(float[2][2]);
                break;
            case ResourceDataType::Matrix3x3:
                Size = sizeof(float[3][3]);
                break;
            case ResourceDataType::Matrix4x4:
                Size = sizeof(float[4][4]);
                break;
            }

            if (Dims > 1)
            {
                Size = Size * Dims;
            }
        }
        return Size;
    }

    uint32_t PropertyDescriptor::AdvanceSize(
        uint32_t Size)
    {
        uint32_t Offset = InvalidOffset;

        if (Size)
        {
            // if offset + size cant fit in 'UserDataAlignment' bytes, move to next 16 bytes boundary
            if (((m_AlignedSize % UserDataAlignment) + Size > UserDataAlignment))
            {
                m_AlignedSize = Math::AlignUp(m_AlignedSize, UserDataAlignment);
            }
            Offset = m_AlignedSize;
            m_AlignedSize += Size;
        }

        return Offset;
    }

    //

    void PropertyDescriptor::InsertProp(
        const String&      PropName,
        const PropertInfo& Info,
        Rhi::ShaderFlags   Flags)
    {
        uint32_t Size   = GetSize(Info.Type, Info.DataType, Info.Dims);
        uint32_t Offset = AdvanceSize(Size);

        auto& Prop = m_Tree.put_child(PropName, {});
        auto& Meta = Prop.put_child(AME_PROPDESC_METANAME, {});

        Meta.put(AME_PROPDESC_METADIMS, Info.Dims);
        Meta.put(AME_PROPDESC_METATYPE, std::to_underlying(Info.Type));
        Meta.put(AME_PROPDESC_METADATATYPE, std::to_underlying(Info.DataType));
        Meta.put(AME_PROPDESC_METAOFFSET, Offset);

        m_ShaderBits.Set(Flags);
    }

    void PropertyDescriptor::InsertStruct(
        const String&             PropName,
        const PropertyDescriptor& Descriptor,
        Rhi::ShaderFlags          Flags)
    {
#ifdef AME_DEBUG
        Log::Renderer().Assert(Descriptor.GetResourceCount() == 0, "Structs cannot contain resources");
#endif

        PadToBoundaries();

        uint32_t Size   = Descriptor.GetStructSize();
        uint32_t Offset = AdvanceSize(Size);

        auto& Prop = m_Tree.put_child(PropName, Descriptor.m_Tree);
        auto& Meta = Prop.put_child(AME_PROPDESC_METANAME, {});

        Meta.put(AME_PROPDESC_METADIMS, 1);
        Meta.put(AME_PROPDESC_METATYPE, std::to_underlying(ResourceType::Struct));
        Meta.put(AME_PROPDESC_METADATATYPE, std::to_underlying(ResourceDataType::None));
        Meta.put(AME_PROPDESC_METAOFFSET, Offset);

        TraverseAppendOffset(Prop, Offset);
        m_ShaderBits.Set(Flags);
    }

    //

    void PropertyDescriptor::TraverseAppendOffset(
        PropertyTree& Subtree,
        uint32_t      Offset)
    {
        std::stack<Ref<PropertyTree>> Trees;

        for (auto& [Name, Child] : Subtree)
        {
            if (Name != AME_PROPDESC_METANAME)
            {
                Trees.push(Child);
            }
        }

        while (!Trees.empty())
        {
            auto& Child = Trees.top().get();
            Trees.pop();

            auto& Meta = Child.get_child(AME_PROPDESC_METANAME);
            auto  Type = static_cast<ResourceType>(Meta.get<std::underlying_type_t<ResourceType>>(AME_PROPDESC_METATYPE));

            Meta.put(AME_PROPDESC_METAOFFSET, Meta.get<uint32_t>(AME_PROPDESC_METAOFFSET) + Offset);

            if (Type == ResourceType::Struct)
            {
                for (auto& [Name, SubChild] : Child)
                {
                    if (Name != AME_PROPDESC_METANAME)
                    {
                        Trees.push(SubChild);
                    }
                }
            }
        }
    }

    void PropertyDescriptor::InsertPadding(
        uint32_t Size)
    {
        m_AlignedSize += Size;
    }

    void PropertyDescriptor::PadToBoundaries()
    {
        m_AlignedSize = Math::AlignUp(m_AlignedSize, UserDataAlignment);
    }
} // namespace Ame::Gfx::Shading
