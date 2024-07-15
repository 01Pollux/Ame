#pragma once

#include <potto/potto.h>
#include <potto/pottoptr.hpp>
#include <potto/pottointerface.hpp>
#include <potto/module/pottoobject.hpp>

namespace Potto
{
    /// <summary>
    /// Initialize all potto objects in current module.
    /// </summary>
    void Initialize();
} // namespace Potto

#define IID_PTR(Obj) decltype(Obj)::element_type::IID(), std::bit_cast<void**>(&Obj)

namespace Ame
{
    namespace Pt = Potto;

    using PtError = Pt::POTTO_ERROR;
    using PtUID   = Pt::PottoUuid;

    template<typename Ty>
    class Ptr : public Pt::PottoPtr<Ty>
    {
    public:
        using Pt::PottoPtr<Ty>::PottoPtr;
        using Pt::PottoPtr<Ty>::operator=;
        using Pt::PottoPtr<Ty>::operator bool;
        using Pt::PottoPtr<Ty>::operator->;
        using Pt::PottoPtr<Ty>::operator*;
        using Pt::PottoPtr<Ty>::get;

        using element_type = typename Pt::PottoPtr<Ty>::element_type;

    public:
        PtError create(const char* clsid)
        {
            return Pt::CreateInstance(clsid, element_type::IID(), std::bit_cast<void**>(this));
        }

        PtError as(const PtUID& riid, void** pp) const
        {
            return get()->QueryInterface(riid, pp);
        }
    };
} // namespace Ame

/// <summary>
/// The begin of a single threaded potto object.
/// </summary>
#define AME_ST_OBJECT(name)                  \
    class POTTO_DECLARE_NOVTABLE name        \
        : public Potto::PottoObjectRootBase, \
          public Potto::PottoObjectClass<name>

/// <summary>
/// The begin of a multiple threaded potto object.
/// </summary>
#define AME_MT_OBJECT(name)                            \
    class POTTO_DECLARE_NOVTABLE name                  \
        : public Potto::PottoThreadSafeObjectRootBase, \
          public Potto::PottoObjectClass<name>

#define AME_DECL_CLASS(name, clsid) constexpr const char CLSID_##name[] = clsid
#define AME_DECL_IID(name, riid)    constexpr const char IID_##name[] = riid
#define AME_INTERFACE_OF(Ty)        POTTO_IMPLEMENT_INTERFACE(Ty)

/// <summary>
/// The begin of a interface.
/// </summary>
#define AME_INTERFACE_BEGIN_EX(iid, name, ...)        \
    const char* const            IID_##name = iid;    \
    class POTTO_DECLARE_NOVTABLE name : __VA_ARGS__   \
    {                                                 \
    public:                                           \
        virtual ~name(){};                            \
        static const Potto::PottoUuid& IID()          \
        {                                             \
            static Potto::PottoUuid iid_(IID_##name); \
            return iid_;                              \
        }
#define AME_INTERFACE_BEGIN(uuid, Ty) AME_INTERFACE_BEGIN_EX(uuid, Ty, public Pt::IUnknown)

#define AME_INTERFACE_METHOD(ret, nameArgs) POTTO_METHOD(ret, nameArgs)
#define AME_INTERFACE_END()                 POTTO_INTERFACE_END

/// <summary>
/// The begin of the component entry map.
/// </summary>
#define AME_BEGIN_COMPONENT_MAP(c, clsid)                                                     \
    const static Potto::PottoUuid& IID()                                                      \
    {                                                                                         \
        static Potto::PottoUuid riid_(clsid);                                                 \
        return riid_;                                                                         \
    };                                                                                        \
    Potto::POTTO_ERROR _InternalQueryInterface(const Potto::PottoUuid& iid, void** ppvObject) \
    {                                                                                         \
        return InternalQueryInterface(this, _GetEntries(), iid, ppvObject);                   \
    }                                                                                         \
    const static Potto::InterfaceEntry* _GetEntries()                                         \
    {                                                                                         \
        typedef c                          _PottoObjectType;                                  \
        static const Potto::InterfaceEntry _entries[] = {

/// <summary>
/// The begin of the interface entry map.
/// </summary>
#define AME_BEGIN_INTERFACE_MAP(c, clsid)                                                     \
    const static Potto::PottoUuid& CLSID()                                                    \
    {                                                                                         \
        static Potto::PottoUuid clsid_(clsid);                                                \
        return clsid_;                                                                        \
    };                                                                                        \
    Potto::POTTO_ERROR _InternalQueryInterface(const Potto::PottoUuid& iid, void** ppvObject) \
    {                                                                                         \
        return InternalQueryInterface(this, _GetEntries(), iid, ppvObject);                   \
    }                                                                                         \
    const static Potto::InterfaceEntry* _GetEntries()                                         \
    {                                                                                         \
        typedef c                          _PottoObjectType;                                  \
        static const Potto::InterfaceEntry _entries[] = {

#define AME_INTERFACE_ENTRY(Ty)  POTTO_INTERFACE_ENTRY(Ty)
#define AME_COMPONENT_ENTRY(Obj) { decltype(Obj)::element_type::CLSID(), offsetof(_PottoObjectType, Obj) },
#define AME_END_INTERFACE_MAP()  POTTO_END_INTERFACE_MAP()
#define AME_END_COMPONENT_MAP()  POTTO_END_INTERFACE_MAP()

#define AME_CLASS_REGISTER_EX(Ty, clsid) static inline Potto::PottoClassRegister<Ty> _##Ty##_autoregister(#Ty, clsid)
#define AME_CLASS_REGISTER(Ty)           AME_CLASS_REGISTER_EX(Ty, Ty::CLSID())