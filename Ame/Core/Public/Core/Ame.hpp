#pragma once

#include <memory>
#include <optional>
#include <tuple>
#include <variant>
#include <expected>

#include <Core/Allocator.hpp>
#include <Core/Ame.Debug.hpp>

namespace concurrencpp
{
    class runtime;
} // namespace concurrencpp

namespace Ame
{
    namespace Co = concurrencpp;

    template<class... ArgsTy>
    struct VariantVisitor : ArgsTy...
    {
        using ArgsTy::operator()...;
    };
} // namespace Ame


namespace Ame
{
    template<typename Ty>
    using Ptr = std::shared_ptr<Ty>;

    template<typename Ty>
    using RefPtr = std::weak_ptr<Ty>;

    template<typename Ty, typename _Dx = std::default_delete<Ty>>
    using UPtr = std::unique_ptr<Ty, _Dx>;

    template<typename Ty>
    using Ref = std::reference_wrapper<Ty>;

    template<typename Ty>
    using CRef = std::reference_wrapper<const Ty>;

    template<typename Ty>
    using Opt = std::optional<Ty>;

    template<typename Ty, typename ErrTy = std::exception_ptr>
    using Expect = std::expected<Ty, ErrTy>;

    template<typename... ArgsTy>
    using Tuple = std::tuple<ArgsTy...>;

    template<typename First, typename Second>
    using Pair = std::pair<First, Second>;
} // namespace Ame
