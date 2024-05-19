#pragma once

#include <Core/Allocator.hpp>

#include <concepts>
#include <utility>
#include <memory>
#include <optional>
#include <expected>

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

    struct NoBaseImpl
    {
    };

    class NonCopyable : NoBaseImpl
    {
    public:
        NonCopyable()  = default;
        ~NonCopyable() = default;

        NonCopyable(const NonCopyable&)            = delete;
        NonCopyable& operator=(const NonCopyable&) = delete;
    };

    class NonMovable : NoBaseImpl
    {
    public:
        NonMovable()  = default;
        ~NonMovable() = default;

        NonMovable(NonMovable&&)            = delete;
        NonMovable& operator=(NonMovable&&) = delete;
    };
} // namespace Ame

template<typename ToTy, typename FromTy>
[[nodiscard]] ToTy validate_cast(FromTy* From)
{
#ifdef AME_DEBUG
    return dynamic_cast<ToTy>(From);
#else
    return static_cast<ToTy>(From);
#endif
}

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

namespace Ame::DbgImpl
{
#ifdef _MSC_VER

    [[noreturn]] inline void debug_break()
    {
        __debugbreak();
    }

#else

#ifdef __cplusplus
    extern "C"
    {
#endif

#define DEBUG_BREAK_USE_TRAP_INSTRUCTION 1
#define DEBUG_BREAK_USE_BULTIN_TRAP      2
#define DEBUG_BREAK_USE_SIGTRAP          3

#if defined(__i386__) || defined(__x86_64__)
#define DEBUG_BREAK_IMPL DEBUG_BREAK_USE_TRAP_INSTRUCTION
        __inline__ static void trap_instruction()
        {
            __asm__ volatile("int $0x03");
        }
#elif defined(__thumb__)
#define DEBUG_BREAK_IMPL DEBUG_BREAK_USE_TRAP_INSTRUCTION
    /* FIXME: handle __THUMB_INTERWORK__ */
    __attribute__((always_inline)) __inline__ static void trap_instruction()
    {
        /* See 'arm-linux-tdep.c' in GDB source.
         * Both instruction sequences below work. */
#if 1
        /* 'eabi_linux_thumb_le_breakpoint' */
        __asm__ volatile(".inst 0xde01");
#else
        /* 'eabi_linux_thumb2_le_breakpoint' */
        __asm__ volatile(".inst.w 0xf7f0a000");
#endif

        /* Known problem:
         * After a breakpoint hit, can't 'stepi', 'step', or 'continue' in GDB.
         * 'step' would keep getting stuck on the same instruction.
         *
         * Workaround: use the new GDB commands 'debugbreak-step' and
         * 'debugbreak-continue' that become available
         * after you source the script from GDB:
         *
         * $ gdb -x debugbreak-gdb.py <... USUAL ARGUMENTS ...>
         *
         * 'debugbreak-step' would jump over the breakpoint instruction with
         * roughly equivalent of:
         * (gdb) set $instruction_len = 2
         * (gdb) tbreak *($pc + $instruction_len)
         * (gdb) jump   *($pc + $instruction_len)
         */
    }
#elif defined(__arm__) && !defined(__thumb__)
#define DEBUG_BREAK_IMPL DEBUG_BREAK_USE_TRAP_INSTRUCTION
    __attribute__((always_inline)) __inline__ static void trap_instruction()
    {
        /* See 'arm-linux-tdep.c' in GDB source,
         * 'eabi_linux_arm_le_breakpoint' */
        __asm__ volatile(".inst 0xe7f001f0");
        /* Known problem:
         * Same problem and workaround as Thumb mode */
    }
#elif defined(__aarch64__) && defined(__APPLE__)
#define DEBUG_BREAK_IMPL DEBUG_BREAK_USE_BULTIN_DEBUGTRAP
#elif defined(__aarch64__)
#define DEBUG_BREAK_IMPL DEBUG_BREAK_USE_TRAP_INSTRUCTION
    __attribute__((always_inline)) __inline__ static void trap_instruction()
    {
        /* See 'aarch64-tdep.c' in GDB source,
         * 'aarch64_default_breakpoint' */
        __asm__ volatile(".inst 0xd4200000");
    }
#elif defined(__powerpc__)
    /* PPC 32 or 64-bit, big or little endian */
#define DEBUG_BREAK_IMPL DEBUG_BREAK_USE_TRAP_INSTRUCTION
    __attribute__((always_inline)) __inline__ static void trap_instruction()
    {
        /* See 'rs6000-tdep.c' in GDB source,
         * 'rs6000_breakpoint' */
        __asm__ volatile(".4byte 0x7d821008");

        /* Known problem:
         * After a breakpoint hit, can't 'stepi', 'step', or 'continue' in GDB.
         * 'step' stuck on the same instruction ("twge r2,r2").
         *
         * The workaround is the same as ARM Thumb mode: use debugbreak-gdb.py
         * or manually jump over the instruction. */
    }
#elif defined(__riscv)
    /* RISC-V 32 or 64-bit, whether the "C" extension
     * for compressed, 16-bit instructions are supported or not */
#define DEBUG_BREAK_IMPL DEBUG_BREAK_USE_TRAP_INSTRUCTION
    __attribute__((always_inline)) __inline__ static void trap_instruction()
    {
        /* See 'riscv-tdep.c' in GDB source,
         * 'riscv_sw_breakpoint_from_kind' */
        __asm__ volatile(".4byte 0x00100073");
    }
#else
#define DEBUG_BREAK_IMPL DEBUG_BREAK_USE_SIGTRAP
#endif

#ifndef DEBUG_BREAK_IMPL
#error "debugbreak.h is not supported on this target"
#elif DEBUG_BREAK_IMPL == DEBUG_BREAK_USE_TRAP_INSTRUCTION
    __attribute__((always_inline)) __inline__ static void debug_break()
    {
        trap_instruction();
    }
#elif DEBUG_BREAK_IMPL == DEBUG_BREAK_USE_BULTIN_DEBUGTRAP
    __attribute__((always_inline)) __inline__ static void debug_break()
    {
        __builtin_debugtrap();
    }
#elif DEBUG_BREAK_IMPL == DEBUG_BREAK_USE_BULTIN_TRAP
    __attribute__((always_inline)) __inline__ static void debug_break()
    {
        __builtin_trap();
    }
#elif DEBUG_BREAK_IMPL == DEBUG_BREAK_USE_SIGTRAP
#include <signal.h>
    __attribute__((always_inline)) __inline__ static void debug_break()
    {
        raise(SIGTRAP);
    }
#else
#error "invalid DEBUG_BREAK_IMPL value"
#endif

#ifdef __cplusplus
    }
#endif

#endif /* ifdef _MSC_VER */
} // namespace Ame::DbgImpl

#define AME_DEBUG_BREAK Ame::DbgImpl::debug_break()