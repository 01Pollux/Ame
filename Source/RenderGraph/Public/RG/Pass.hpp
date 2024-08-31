#pragma once

#include <functional>

#include <Rg/Core.hpp>
#include <Rg/Resolver.hpp>
#include <Rg/ResourceStorage.hpp>

#include <Math/Common.hpp>

namespace Ame::Rg
{
    class Pass
    {
    public:
        using BuildFuncType   = std::move_only_function<void(Resolver&)>;
        using ExecuteFuncType = std::move_only_function<void(const ResourceStorage&, Dg::IDeviceContext*)>;

        Pass() = default;

        Pass(const Pass&)                = default;
        Pass& operator=(const Pass&)     = default;
        Pass(Pass&&) noexcept            = default;
        Pass& operator=(Pass&&) noexcept = default;

        virtual ~Pass() = default;

        /// <summary>
        /// Set the queue type of the pass
        /// </summary>
        Pass& Flags(PassFlags flags, bool enable = true)
        {
            using namespace EnumBitOperators;
            if (enable)
            {
                m_Flags |= flags;
            }
            else
            {
                m_Flags &= ~flags;
            }
            return *this;
        }

        Pass& Color(Math::Color4 color)
        {
#ifndef AME_DIST
            m_Color = color;
#endif
            return *this;
        }

        /// <summary>
        /// Get the name of the pass
        /// </summary>
        Pass& Name(StringView name)
        {
#ifndef AME_DIST
            m_Name = name;
#endif
            return *this;
        }

        /// <summary>
        /// Initializes a build callback
        /// </summary>
        Pass& Build(BuildFuncType&& buildFunc)
        {
            m_BuildFunc = std::move(buildFunc);
            return *this;
        }

        /// <summary>
        /// Initializes an execute callback
        /// </summary>
        Pass& Execute(ExecuteFuncType&& executeFunc)
        {
            m_ExecuteFunc = std::move(executeFunc);
            return *this;
        }

#ifndef AME_DIST
        /// <summary>
        /// Get the color of the pass
        /// </summary>
        [[nodiscard]] const Math::Color4& GetColor() const
        {
            return m_Color;
        }

        /// <summary>
        /// Get the color of the pass
        /// </summary>
        [[nodiscard]] const float* GetColorPtr() const
        {
            return m_Color.a() != 0.f ? m_Color.data() : nullptr;
        }

        /// <summary>
        /// Get the name of the pass
        /// </summary>
        [[nodiscard]] StringView GetName() const
        {
            return m_Name;
        }
#else
        /// <summary>
        /// Get the color of the pass
        /// </summary>
        [[nodiscard]] Math::Color4 GetColor() const
        {
            return {};
        }

        /// <summary>
        /// Get the color of the pass
        /// </summary>
        [[nodiscard]] const float* GetColorPtr() const
        {
            return nullptr;
        }

        /// <summary>
        /// Get the name of the pass
        /// </summary>
        [[nodiscard]] StringView GetName() const
        {
            return "";
        }
#endif

        /// <summary>
        /// Get the flags of the pass
        /// </summary>
        [[nodiscard]] PassFlags GetFlags() const
        {
            return m_Flags;
        }

        /// <summary>
        /// Get the queue type of the pass
        /// </summary>
        [[nodiscard]] PassFlags GetQueueType() const
        {
            using namespace EnumBitOperators;
            return m_Flags & PassFlags::TypeMask;
        }

        /// <summary>
        /// Build render pass
        /// </summary>
        [[nodiscard]] void DoBuild(Resolver& resolver)
        {
            if (m_BuildFunc) [[likely]]
            {
                m_BuildFunc(resolver);
            }
        }

        /// <summary>
        /// Execute render pass
        /// </summary>
        void DoExecute(const ResourceStorage& storage, Dg::IDeviceContext* deviceContext)
        {
            if (m_ExecuteFunc) [[likely]]
            {
                m_ExecuteFunc(storage, deviceContext);
            }
        }

    private:
        BuildFuncType   m_BuildFunc;
        ExecuteFuncType m_ExecuteFunc;
        PassFlags       m_Flags = PassFlags::None;

#ifndef AME_DIST
        String       m_Name = "Unnamed";
        Math::Color4 m_Color;
#endif
    };

    //

    template<typename Ty> class TypedPass : public Pass
    {
    public:
        using BuildFuncType   = std::move_only_function<void(Ty&, Resolver&)>;
        using ExecuteFuncType = std::move_only_function<void(const Ty&, const ResourceStorage&, Dg::IDeviceContext*)>;

        /// <summary>
        /// Initializes a build callback
        /// </summary>
        TypedPass& Build(BuildFuncType&& buildFunc)
        {
            Pass::Build(std::bind_front(std::move(buildFunc), std::ref(m_Data)));
            return *this;
        }

        /// <summary>
        /// Initializes an execute callback
        /// </summary>
        TypedPass& Execute(ExecuteFuncType&& executeFunc)
        {
            Pass::Execute(std::bind_front(std::move(executeFunc), std::cref(m_Data)));
            return *this;
        }

    private:
        Ty m_Data;
    };

    template<> class TypedPass<void> : public Pass
    {
    public:
        using BuildFuncType   = Pass::BuildFuncType;
        using ExecuteFuncType = Pass::ExecuteFuncType;

        /// <summary>
        /// Initializes a build callback
        /// </summary>
        TypedPass& Build(BuildFuncType&& buildFunc)
        {
            Pass::Build(std::forward<BuildFuncType>(buildFunc));
            return *this;
        }

        /// <summary>
        /// Initializes an execute callback
        /// </summary>
        TypedPass& Execute(ExecuteFuncType&& executeFunc)
        {
            Pass::Execute(std::forward<ExecuteFuncType>(executeFunc));
            return *this;
        }
    };

    using UntypedPass = TypedPass<void>;
} // namespace Ame::Rg