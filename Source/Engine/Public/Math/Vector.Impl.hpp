#pragma once

#include <type_traits>
#include <limits>
#include <algorithm>
#include <array>
#include <bit>
#include <cmath>

#include <Math/Util.hpp>

namespace Ame::Math::Impl
{
#define AME_VECTOR_IMPL_MATH_OP(VecType, Symbol)                                                                       \
    VecType& operator##Symbol##=(const VecType & other) noexcept;                                                      \
    VecType& operator##Symbol##=(value_type val) noexcept;                                                             \
    [[nodiscard]] VecType operator##Symbol(const VecType& other) const noexcept                                        \
    {                                                                                                                  \
        VecType Tmp   = *this;                                                                                         \
        Tmp##Symbol## = other;                                                                                         \
        return Tmp;                                                                                                    \
    }                                                                                                                  \
    [[nodiscard]] VecType operator##Symbol(value_type val) const noexcept                                              \
    {                                                                                                                  \
        VecType Tmp   = *this;                                                                                         \
        Tmp##Symbol## = val;                                                                                           \
        return Tmp;                                                                                                    \
    }

#define AME_VECTOR_IMPL_ACCESSOR(Index, Name)                                                                          \
    constexpr value_type Name() const noexcept                                                                         \
    {                                                                                                                  \
        return (*this)[Index];                                                                                         \
    }                                                                                                                  \
                                                                                                                       \
    constexpr value_type& Name() noexcept                                                                              \
    {                                                                                                                  \
        return (*this)[Index];                                                                                         \
    }

#define AME_VECTOR_IMPL_MATHVEC_DECLARATIONS(VecType)                                                                  \
                                                                                                                       \
    [[nodiscard]] bool IsNan() const noexcept;                                                                         \
                                                                                                                       \
    void Negate() noexcept;                                                                                            \
                                                                                                                       \
    [[nodiscard]] value_type Dot(const VecType& other) const noexcept;                                                 \
                                                                                                                       \
    [[nodiscard]] value_type LengthSqr() const noexcept;                                                               \
                                                                                                                       \
    [[nodiscard]] value_type Length() const noexcept;                                                                  \
                                                                                                                       \
    [[nodiscard]] bool IsGreaterThan(const VecType& other) const noexcept;                                             \
                                                                                                                       \
    [[nodiscard]] bool IsLessThan(const VecType& other) const noexcept;                                                \
                                                                                                                       \
    void Normalize() noexcept;                                                                                         \
                                                                                                                       \
    [[nodiscard]] bool NearEqual(                                                                                      \
        const VecType& other, value_type tolerance = std::numeric_limits<value_type>::epsilon()) const noexcept;       \
                                                                                                                       \
    [[nodiscard]] bool Within(const VecType& min, const VecType& max) const noexcept;                                  \
                                                                                                                       \
    /* Multiply, add, and assign to this (ie: *this = *this * a + b ). */                                              \
    void MultAdd(const VecType& a, const VecType& b) noexcept;                                                         \
                                                                                                                       \
    VecType& operator=(value_type val) noexcept;                                                                       \
                                                                                                                       \
    [[nodiscard]] VecType operator-() const noexcept;                                                                  \
                                                                                                                       \
    [[nodiscard]] VecType min(const VecType& other) const noexcept;                                                    \
                                                                                                                       \
    [[nodiscard]] VecType max(const VecType& other) const noexcept;                                                    \
                                                                                                                       \
    [[nodiscard]] value_type DistanceTo(const VecType& other) const noexcept                                           \
    {                                                                                                                  \
        return (other - *this).Length();                                                                               \
    }                                                                                                                  \
                                                                                                                       \
    [[nodiscard]] value_type DistanceToSqr(const VecType& other) const noexcept                                        \
    {                                                                                                                  \
        return (other - *this).LengthSqr();                                                                            \
    }                                                                                                                  \
                                                                                                                       \
    AME_VECTOR_IMPL_MATH_OP(VecType, +);                                                                               \
    AME_VECTOR_IMPL_MATH_OP(VecType, -);                                                                               \
    AME_VECTOR_IMPL_MATH_OP(VecType, /);                                                                               \
    AME_VECTOR_IMPL_MATH_OP(VecType, *)

    //

    template<typename Ty, size_t Size> class MVector
    {
    public:
        using value_type      = Ty;
        using reference       = value_type&;
        using const_reference = const value_type&;
        using pointer         = Ty*;
        using const_pointer   = const Ty*;

        static constexpr size_t array_size = Size;
        using array_type                   = std::array<value_type, Size>;
        using difference_type              = array_type::difference_type;

        using iterator       = array_type::iterator;
        using const_iterator = array_type::const_iterator;

        using reverse_iterator       = array_type::reverse_iterator;
        using const_reverse_iterator = array_type::const_reverse_iterator;

    public:
        constexpr MVector() = default;

        constexpr MVector(std::initializer_list<value_type> list) noexcept
        {
            std::copy_n(list.begin(), std::min(list.size(), size()), begin());
        }

        constexpr MVector(const array_type& arr) noexcept : m_Data{ arr }
        {
        }

        template<typename OtherTy, size_t OtherSize>
            requires std::negation_v<std::is_same<MVector, MVector<OtherTy, OtherSize>>>
        constexpr MVector(const MVector<OtherTy, OtherSize>& other) noexcept
        {
            if constexpr (std::is_same_v<value_type, OtherTy>)
            {
                std::copy_n(other.begin(), std::min(other.size(), size()), begin());
            }
            else
            {
                auto meit = begin();
                for (auto it = other.begin(), end = it + std::min(other.size(), size()); it != end; ++it, ++meit)
                    *meit = static_cast<value_type>(*it);
            }
        }

    public:
        constexpr auto operator<=>(const MVector&) const noexcept = default;
        constexpr auto operator==(const MVector& other) const noexcept
        {
            return std::equal(begin(), end(), other.begin(), other.end());
        }

    public:
        [[nodiscard]] constexpr const_reference operator[](difference_type i) const noexcept
        {
            return m_Data[i];
        }
        [[nodiscard]] constexpr reference operator[](difference_type i) noexcept
        {
            return m_Data[i];
        }

        [[nodiscard]] constexpr const_pointer data() const noexcept
        {
            return m_Data.data();
        }

        [[nodiscard]] constexpr pointer data() noexcept
        {
            return m_Data.data();
        }

        [[nodiscard]] constexpr const array_type& get() const noexcept
        {
            return m_Data;
        }

        [[nodiscard]] constexpr array_type& get() noexcept
        {
            return m_Data;
        }

        [[nodiscard]] constexpr size_t size() const noexcept
        {
            return array_size;
        }

        [[nodiscard]] constexpr void ceil()
        {
            for (auto& v : m_Data)
            {
                v = static_cast<value_type>(std::ceil(v));
            }
        }

        [[nodiscard]] constexpr void floor()
        {
            for (auto& v : m_Data)
            {
                v = static_cast<value_type>(std::floor(v));
            }
        }

        template<typename OtherTy>
            requires(sizeof(OtherTy) == sizeof(Ty) * size())
        [[nodiscard]] constexpr const OtherTy& To() const noexcept
        {
            return *std::bit_cast<const OtherTy*>(data());
        }

        template<typename OtherTy>
            requires(sizeof(OtherTy) == sizeof(Ty) * size())
        [[nodiscard]] constexpr OtherTy& To() noexcept
        {
            return *std::bit_cast<OtherTy*>(data());
        }

        constexpr void UnsafeCopyFrom(const_pointer pval)
        {
            for (reference v : m_Data)
            {
                v = *pval;
                ++pval;
            }
        }

        constexpr void UnsafeCopyTo(pointer pval) const
        {
            for (const_reference v : m_Data)
            {
                *pval = v;
                ++pval;
            }
        }

    public:
        constexpr void fill(const_reference val)
        {
            m_Data.fill(val);
        }

        constexpr void swap(array_type& other) noexcept(std::is_nothrow_swappable_v<Ty>)
        {
            m_Data.swap(other);
        }

        [[nodiscard]] constexpr iterator begin() noexcept
        {
            return m_Data.begin();
        }

        [[nodiscard]] constexpr const_iterator begin() const noexcept
        {
            return m_Data.begin();
        }

        [[nodiscard]] constexpr iterator end() noexcept
        {
            return m_Data.end();
        }

        [[nodiscard]] constexpr const_iterator end() const noexcept
        {
            return m_Data.end();
        }

        [[nodiscard]] constexpr reverse_iterator rbegin() noexcept
        {
            return m_Data.rbegin();
        }

        [[nodiscard]] constexpr const_reverse_iterator rbegin() const noexcept
        {
            return m_Data.rbegin();
        }

        [[nodiscard]] constexpr reverse_iterator rend() noexcept
        {
            return m_Data.rend();
        }

        [[nodiscard]] constexpr const_reverse_iterator rend() const noexcept
        {
            return m_Data.rend();
        }

        [[nodiscard]] constexpr const_iterator cbegin() const noexcept
        {
            return begin();
        }

        [[nodiscard]] constexpr const_iterator cend() const noexcept
        {
            return end();
        }

        [[nodiscard]] constexpr const_reverse_iterator crbegin() const noexcept
        {
            return rbegin();
        }

        [[nodiscard]] constexpr const_reverse_iterator crend() const noexcept
        {
            return rend();
        }

    private:
        array_type m_Data{};
    };

    //

    template<typename Ty, typename... RestTy> MVector(Ty, RestTy...) -> MVector<Ty, 1 + sizeof...(RestTy)>;

    //

    template<typename Ty> class VectorConstants
    {
        using value_type = typename Ty::value_type;

    private:
        static constexpr value_type EpsilonValue  = std::numeric_limits<value_type>::epsilon();
        static constexpr value_type InfinityValue = std::numeric_limits<value_type>::infinity();
        static constexpr value_type NaNValue      = std::numeric_limits<value_type>::quiet_NaN();
        static constexpr value_type MaxValue      = std::numeric_limits<value_type>::max();
        static constexpr value_type MinValue      = std::numeric_limits<value_type>::min();

    public:
        static inline constexpr Ty One  = Ty{ value_type(1), value_type(1), value_type(1), value_type(1) };
        static inline constexpr Ty Zero = Ty{ value_type(0), value_type(0), value_type(0), value_type(0) };

        static inline constexpr Ty Epsilon  = Ty{ EpsilonValue, EpsilonValue, EpsilonValue, EpsilonValue };
        static inline constexpr Ty Infinity = Ty{ InfinityValue, InfinityValue, InfinityValue, InfinityValue };
        static inline constexpr Ty NaN      = Ty{ NaNValue, NaNValue, NaNValue, NaNValue };
        static inline constexpr Ty Max      = Ty{ MaxValue, MaxValue, MaxValue, MaxValue };
        static inline constexpr Ty Min      = Ty{ MinValue, MinValue, MinValue, MinValue };

        static inline constexpr Ty Up       = Ty{ value_type(0), value_type(1), value_type(0), value_type(0) };
        static inline constexpr Ty Down     = Ty{ value_type(0), value_type(-1), value_type(0), value_type(0) };
        static inline constexpr Ty Left     = Ty{ value_type(-1), value_type(0), value_type(0), value_type(0) };
        static inline constexpr Ty Right    = Ty{ value_type(1), value_type(0), value_type(0), value_type(0) };
        static inline constexpr Ty Forward  = Ty{ value_type(0), value_type(0), value_type(1), value_type(0) };
        static inline constexpr Ty Backward = Ty{ value_type(0), value_type(0), value_type(-1), value_type(0) };
    };
} // namespace Ame::Math::Impl
