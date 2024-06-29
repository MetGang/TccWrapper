/*
    Convenient header-only C++17 wrapper to use with embedded Tiny C Compiler (tcc).

    Define TW_USE_EXCEPTIONS to use with_state/invoke methods
    Define TW_USE_OPTIONAL to use opt_with_state/opt_invoke methods

    Created by Patrick Stritch
*/

#pragma once

#if (defined(_MSVC_LANG) && _MSVC_LANG >= 201703L) || __cplusplus >= 201703L

#define TW_VERSION_MAJOR 1
#define TW_VERSION_MINOR 0
#define TW_VERSION_PATCH 0

// C++
#include <cstdint>
#include <cstring>
#include <utility>

#if defined(TW_USE_EXCEPTIONS)
#include <stdexcept>
#include <string>
#endif

#if defined(TW_USE_OPTIONAL)
#include <optional>
#endif

// tinycc
#include <libtcc.h>

namespace tw
{
    namespace priv
    {
        namespace traits
        {
            /// Trait to recognize if F is a function
            template <typename F>
            inline constexpr bool Function_v = std::is_function_v<F>;

            /// Trait to recognize if FP is a function pointer
            template <typename FP>
            inline constexpr bool FunctionPtr_v = std::is_function_v<std::remove_pointer_t<FP>>;

            /// Trait to recognize if MP is a method (member function) pointer
            template <typename MP>
            inline constexpr bool MethodPtr_v = std::is_member_function_pointer_v<MP>;

            /// Trait to recognize if F is invokable with Args...
            template <typename F, typename... Args>
            inline constexpr bool InvokableWith_v = std::is_invocable_v<F, Args...>;

            /// Trait to recognize if From is bit-castable to To
            template <typename To, typename From>
            inline constexpr bool BitCastable_v = (sizeof(To) == sizeof(From)) &&
                                                  std::is_trivially_copyable_v<From> &&
                                                  std::is_trivial_v<To>;
        }

        /// Fallback helper for more readable template error messages
        template <typename...>
        inline constexpr bool error = false;

        /// Type-safe reinterpret_cast equivalent that follows type aliasing rules
        template <typename To, typename From>
        To bit_cast(From const& src) noexcept
        {
            if constexpr (traits::BitCastable_v<To, From>)
            {
                To dst;
                std::memcpy(&dst, &src, sizeof(To));
                return dst;
            }
            else
            {
                static_assert(error<To, From>, "Type requirements to perform bit cast are not met!");
            }
        }

        /// Helper struct holder for as_free_function method
        template <auto vMethodPtr, bool vIsNoexcept, bool vIsCVariadic, typename Class, typename Ret, typename... Args>
        struct MethodConverterBase
        {
            /// Convert method pointer to C-like function pointer
            static constexpr auto as_free_function() noexcept
            {
                if constexpr (vIsCVariadic)
                {
                    static_assert(error<Class>, "C-like variadic arguments in method are not supported!");
                }
                else
                {
                    return +[](std::remove_reference_t<Class>* ptr, Args... args) noexcept(vIsNoexcept) -> Ret {
                        return (std::forward<Class>(*ptr).*vMethodPtr)(std::forward<Args>(args)...);
                    };
                }
            }
        };

        /// Basic template struct for MethodConverter
        template <typename, auto>
        struct MethodConverter;

        /// MethodConverter specialization for `Ret(Class::*)(Args...)` signature
        template <typename Class, typename Ret, typename... Args, auto vMethodPtr>
        struct MethodConverter<Ret(Class::*)(Args...), vMethodPtr>
            : MethodConverterBase<vMethodPtr, false, false, Class, Ret, Args...> {};

        /// MethodConverter specialization for `Ret(Class::*)(Args...) const` signature
        template <typename Class, typename Ret, typename... Args, auto vMethodPtr>
        struct MethodConverter<Ret(Class::*)(Args...) const, vMethodPtr>
            : MethodConverterBase<vMethodPtr, false, false, Class const, Ret, Args...> {};

        /// MethodConverter specialization for `Ret(Class::*)(Args...) volatile` signature
        template <typename Class, typename Ret, typename... Args, auto vMethodPtr>
        struct MethodConverter<Ret(Class::*)(Args...) volatile, vMethodPtr>
            : MethodConverterBase<vMethodPtr, false, false, Class volatile, Ret, Args...> {};

        /// MethodConverter specialization for `Ret(Class::*)(Args...) const volatile` signature
        template <typename Class, typename Ret, typename... Args, auto vMethodPtr>
        struct MethodConverter<Ret(Class::*)(Args...) const volatile, vMethodPtr>
            : MethodConverterBase<vMethodPtr, false, false, Class const volatile, Ret, Args...> {};

        /// MethodConverter specialization for `Ret(Class::*)(Args..., ...)` signature
        template <typename Class, typename Ret, typename... Args, auto vMethodPtr>
        struct MethodConverter<Ret(Class::*)(Args..., ...), vMethodPtr>
            : MethodConverterBase<vMethodPtr, false, true, Class, Ret, Args...> {};

        /// MethodConverter specialization for `Ret(Class::*)(Args..., ...) const` signature
        template <typename Class, typename Ret, typename... Args, auto vMethodPtr>
        struct MethodConverter<Ret(Class::*)(Args..., ...) const, vMethodPtr>
            : MethodConverterBase<vMethodPtr, false, true, Class const, Ret, Args...> {};

        /// MethodConverter specialization for `Ret(Class::*)(Args..., ...) volatile` signature
        template <typename Class, typename Ret, typename... Args, auto vMethodPtr>
        struct MethodConverter<Ret(Class::*)(Args..., ...) volatile, vMethodPtr>
            : MethodConverterBase<vMethodPtr, false, true, Class volatile, Ret, Args...> {};

        /// MethodConverter specialization for `Ret(Class::*)(Args..., ...) const volatile` signature
        template <typename Class, typename Ret, typename... Args, auto vMethodPtr>
        struct MethodConverter<Ret(Class::*)(Args..., ...) const volatile, vMethodPtr>
            : MethodConverterBase<vMethodPtr, false, true, Class const volatile, Ret, Args...> {};

        /// MethodConverter specialization for `Ret(Class::*)(Args...) &` signature
        template <typename Class, typename Ret, typename... Args, auto vMethodPtr>
        struct MethodConverter<Ret(Class::*)(Args...) &, vMethodPtr>
            : MethodConverterBase<vMethodPtr, false, false, Class&, Ret, Args...> {};

        /// MethodConverter specialization for `Ret(Class::*)(Args...) const &` signature
        template <typename Class, typename Ret, typename... Args, auto vMethodPtr>
        struct MethodConverter<Ret(Class::*)(Args...) const &, vMethodPtr>
            : MethodConverterBase<vMethodPtr, false, false, Class const&, Ret, Args...> {};

        /// MethodConverter specialization for `Ret(Class::*)(Args...) volatile &` signature
        template <typename Class, typename Ret, typename... Args, auto vMethodPtr>
        struct MethodConverter<Ret(Class::*)(Args...) volatile &, vMethodPtr>
            : MethodConverterBase<vMethodPtr, false, false, Class volatile&, Ret, Args...> {};

        /// MethodConverter specialization for `Ret(Class::*)(Args...) const volatile &` signature
        template <typename Class, typename Ret, typename... Args, auto vMethodPtr>
        struct MethodConverter<Ret(Class::*)(Args...) const volatile &, vMethodPtr>
            : MethodConverterBase<vMethodPtr, false, false, Class const volatile&, Ret, Args...> {};

        /// MethodConverter specialization for `Ret(Class::*)(Args..., ...) &` signature
        template <typename Class, typename Ret, typename... Args, auto vMethodPtr>
        struct MethodConverter<Ret(Class::*)(Args..., ...) &, vMethodPtr>
            : MethodConverterBase<vMethodPtr, false, true, Class&, Ret, Args...> {};

        /// MethodConverter specialization for `Ret(Class::*)(Args..., ...) const &` signature
        template <typename Class, typename Ret, typename... Args, auto vMethodPtr>
        struct MethodConverter<Ret(Class::*)(Args..., ...) const &, vMethodPtr>
            : MethodConverterBase<vMethodPtr, false, true, Class const&, Ret, Args...> {};

        /// MethodConverter specialization for `Ret(Class::*)(Args..., ...) volatile &` signature
        template <typename Class, typename Ret, typename... Args, auto vMethodPtr>
        struct MethodConverter<Ret(Class::*)(Args..., ...) volatile &, vMethodPtr>
            : MethodConverterBase<vMethodPtr, false, true, Class volatile&, Ret, Args...> {};

        /// MethodConverter specialization for `Ret(Class::*)(Args..., ...) const volatile &` signature
        template <typename Class, typename Ret, typename... Args, auto vMethodPtr>
        struct MethodConverter<Ret(Class::*)(Args..., ...) const volatile &, vMethodPtr>
            : MethodConverterBase<vMethodPtr, false, true, Class const volatile&, Ret, Args...> {};

        /// MethodConverter specialization for `Ret(Class::*)(Args...) &&` signature
        template <typename Class, typename Ret, typename... Args, auto vMethodPtr>
        struct MethodConverter<Ret(Class::*)(Args...) &&, vMethodPtr>
            : MethodConverterBase<vMethodPtr, false, false, Class&&, Ret, Args...> {};

        /// MethodConverter specialization for `Ret(Class::*)(Args...) const &&` signature
        template <typename Class, typename Ret, typename... Args, auto vMethodPtr>
        struct MethodConverter<Ret(Class::*)(Args...) const &&, vMethodPtr>
            : MethodConverterBase<vMethodPtr, false, false, Class const&&, Ret, Args...> {};

        /// MethodConverter specialization for `Ret(Class::*)(Args...) volatile &&` signature
        template <typename Class, typename Ret, typename... Args, auto vMethodPtr>
        struct MethodConverter<Ret(Class::*)(Args...) volatile &&, vMethodPtr>
            : MethodConverterBase<vMethodPtr, false, false, Class volatile&&, Ret, Args...> {};

        /// MethodConverter specialization for `Ret(Class::*)(Args...) const volatile &&` signature
        template <typename Class, typename Ret, typename... Args, auto vMethodPtr>
        struct MethodConverter<Ret(Class::*)(Args...) const volatile &&, vMethodPtr>
            : MethodConverterBase<vMethodPtr, false, false, Class const volatile&&, Ret, Args...> {};

        /// MethodConverter specialization for `Ret(Class::*)(Args..., ...) &&` signature
        template <typename Class, typename Ret, typename... Args, auto vMethodPtr>
        struct MethodConverter<Ret(Class::*)(Args..., ...) &&, vMethodPtr>
            : MethodConverterBase<vMethodPtr, false, true, Class&&, Ret, Args...> {};

        /// MethodConverter specialization for `Ret(Class::*)(Args..., ...) const &&` signature
        template <typename Class, typename Ret, typename... Args, auto vMethodPtr>
        struct MethodConverter<Ret(Class::*)(Args..., ...) const &&, vMethodPtr>
            : MethodConverterBase<vMethodPtr, false, true, Class const&&, Ret, Args...> {};

        /// MethodConverter specialization for `Ret(Class::*)(Args..., ...) volatile &&` signature
        template <typename Class, typename Ret, typename... Args, auto vMethodPtr>
        struct MethodConverter<Ret(Class::*)(Args..., ...) volatile &&, vMethodPtr>
            : MethodConverterBase<vMethodPtr, false, true, Class volatile&&, Ret, Args...> {};

        /// MethodConverter specialization for `Ret(Class::*)(Args..., ...) const volatile &&` signature
        template <typename Class, typename Ret, typename... Args, auto vMethodPtr>
        struct MethodConverter<Ret(Class::*)(Args..., ...) const volatile &&, vMethodPtr>
            : MethodConverterBase<vMethodPtr, false, true, Class const volatile&&, Ret, Args...> {};

        /// MethodConverter specialization for `Ret(Class::*)(Args...) noexcept` signature
        template <typename Class, typename Ret, typename... Args, auto vMethodPtr>
        struct MethodConverter<Ret(Class::*)(Args...) noexcept, vMethodPtr>
            : MethodConverterBase<vMethodPtr, true, false, Class, Ret, Args...> {};

        /// MethodConverter specialization for `Ret(Class::*)(Args...) const noexcept` signature
        template <typename Class, typename Ret, typename... Args, auto vMethodPtr>
        struct MethodConverter<Ret(Class::*)(Args...) const noexcept, vMethodPtr>
            : MethodConverterBase<vMethodPtr, true, false, Class const, Ret, Args...> {};

        /// MethodConverter specialization for `Ret(Class::*)(Args...) volatile noexcept` signature
        template <typename Class, typename Ret, typename... Args, auto vMethodPtr>
        struct MethodConverter<Ret(Class::*)(Args...) volatile noexcept, vMethodPtr>
            : MethodConverterBase<vMethodPtr, true, false, Class volatile, Ret, Args...> {};

        /// MethodConverter specialization for `Ret(Class::*)(Args...) const volatile noexcept` signature
        template <typename Class, typename Ret, typename... Args, auto vMethodPtr>
        struct MethodConverter<Ret(Class::*)(Args...) const volatile noexcept, vMethodPtr>
            : MethodConverterBase<vMethodPtr, true, false, Class const volatile, Ret, Args...> {};

        /// MethodConverter specialization for `Ret(Class::*)(Args..., ...) noexcept` signature
        template <typename Class, typename Ret, typename... Args, auto vMethodPtr>
        struct MethodConverter<Ret(Class::*)(Args..., ...) noexcept, vMethodPtr>
            : MethodConverterBase<vMethodPtr, true, true, Class, Ret, Args...> {};

        /// MethodConverter specialization for `Ret(Class::*)(Args..., ...) const noexcept` signature
        template <typename Class, typename Ret, typename... Args, auto vMethodPtr>
        struct MethodConverter<Ret(Class::*)(Args..., ...) const noexcept, vMethodPtr>
            : MethodConverterBase<vMethodPtr, true, true, Class const, Ret, Args...> {};

        /// MethodConverter specialization for `Ret(Class::*)(Args..., ...) volatile noexcept` signature
        template <typename Class, typename Ret, typename... Args, auto vMethodPtr>
        struct MethodConverter<Ret(Class::*)(Args..., ...) volatile noexcept, vMethodPtr>
            : MethodConverterBase<vMethodPtr, true, true, Class volatile, Ret, Args...> {};

        /// MethodConverter specialization for `Ret(Class::*)(Args..., ...) const volatile noexcept` signature
        template <typename Class, typename Ret, typename... Args, auto vMethodPtr>
        struct MethodConverter<Ret(Class::*)(Args..., ...) const volatile noexcept, vMethodPtr>
            : MethodConverterBase<vMethodPtr, true, true, Class const volatile, Ret, Args...> {};

        /// MethodConverter specialization for `Ret(Class::*)(Args...) & noexcept` signature
        template <typename Class, typename Ret, typename... Args, auto vMethodPtr>
        struct MethodConverter<Ret(Class::*)(Args...) & noexcept, vMethodPtr>
            : MethodConverterBase<vMethodPtr, true, false, Class&, Ret, Args...> {};

        /// MethodConverter specialization for `Ret(Class::*)(Args...) const & noexcept` signature
        template <typename Class, typename Ret, typename... Args, auto vMethodPtr>
        struct MethodConverter<Ret(Class::*)(Args...) const & noexcept, vMethodPtr>
            : MethodConverterBase<vMethodPtr, true, false, Class const&, Ret, Args...> {};

        /// MethodConverter specialization for `Ret(Class::*)(Args...) volatile & noexcept` signature
        template <typename Class, typename Ret, typename... Args, auto vMethodPtr>
        struct MethodConverter<Ret(Class::*)(Args...) volatile & noexcept, vMethodPtr>
            : MethodConverterBase<vMethodPtr, true, false, Class volatile&, Ret, Args...> {};

        /// MethodConverter specialization for `Ret(Class::*)(Args...) const volatile & noexcept` signature
        template <typename Class, typename Ret, typename... Args, auto vMethodPtr>
        struct MethodConverter<Ret(Class::*)(Args...) const volatile & noexcept, vMethodPtr>
            : MethodConverterBase<vMethodPtr, true, false, Class const volatile&, Ret, Args...> {};

        /// MethodConverter specialization for `Ret(Class::*)(Args..., ...) & noexcept` signature
        template <typename Class, typename Ret, typename... Args, auto vMethodPtr>
        struct MethodConverter<Ret(Class::*)(Args..., ...) & noexcept, vMethodPtr>
            : MethodConverterBase<vMethodPtr, true, true, Class&, Ret, Args...> {};

        /// MethodConverter specialization for `Ret(Class::*)(Args..., ...) const & noexcept` signature
        template <typename Class, typename Ret, typename... Args, auto vMethodPtr>
        struct MethodConverter<Ret(Class::*)(Args..., ...) const & noexcept, vMethodPtr>
            : MethodConverterBase<vMethodPtr, true, true, Class const&, Ret, Args...> {};

        /// MethodConverter specialization for `Ret(Class::*)(Args..., ...) volatile & noexcept` signature
        template <typename Class, typename Ret, typename... Args, auto vMethodPtr>
        struct MethodConverter<Ret(Class::*)(Args..., ...) volatile & noexcept, vMethodPtr>
            : MethodConverterBase<vMethodPtr, true, true, Class volatile&, Ret, Args...> {};

        /// MethodConverter specialization for `Ret(Class::*)(Args..., ...) const volatile & noexcept` signature
        template <typename Class, typename Ret, typename... Args, auto vMethodPtr>
        struct MethodConverter<Ret(Class::*)(Args..., ...) const volatile & noexcept, vMethodPtr>
            : MethodConverterBase<vMethodPtr, true, true, Class const volatile&, Ret, Args...> {};

        /// MethodConverter specialization for `Ret(Class::*)(Args...) && noexcept` signature
        template <typename Class, typename Ret, typename... Args, auto vMethodPtr>
        struct MethodConverter<Ret(Class::*)(Args...) && noexcept, vMethodPtr>
            : MethodConverterBase<vMethodPtr, true, false, Class&&, Ret, Args...> {};

        /// MethodConverter specialization for `Ret(Class::*)(Args...) const && noexcept` signature
        template <typename Class, typename Ret, typename... Args, auto vMethodPtr>
        struct MethodConverter<Ret(Class::*)(Args...) const && noexcept, vMethodPtr>
            : MethodConverterBase<vMethodPtr, true, false, Class const&&, Ret, Args...> {};

        /// MethodConverter specialization for `Ret(Class::*)(Args...) volatile && noexcept` signature
        template <typename Class, typename Ret, typename... Args, auto vMethodPtr>
        struct MethodConverter<Ret(Class::*)(Args...) volatile && noexcept, vMethodPtr>
            : MethodConverterBase<vMethodPtr, true, false, Class volatile&&, Ret, Args...> {};

        /// MethodConverter specialization for `Ret(Class::*)(Args...) const volatile && noexcept` signature
        template <typename Class, typename Ret, typename... Args, auto vMethodPtr>
        struct MethodConverter<Ret(Class::*)(Args...) const volatile && noexcept, vMethodPtr>
            : MethodConverterBase<vMethodPtr, true, false, Class const volatile&&, Ret, Args...> {};

        /// MethodConverter specialization for `Ret(Class::*)(Args..., ...) && noexcept` signature
        template <typename Class, typename Ret, typename... Args, auto vMethodPtr>
        struct MethodConverter<Ret(Class::*)(Args..., ...) && noexcept, vMethodPtr>
            : MethodConverterBase<vMethodPtr, true, true, Class&&, Ret, Args...> {};

        /// MethodConverter specialization for `Ret(Class::*)(Args..., ...) const && noexcept` signature
        template <typename Class, typename Ret, typename... Args, auto vMethodPtr>
        struct MethodConverter<Ret(Class::*)(Args..., ...) const && noexcept, vMethodPtr>
            : MethodConverterBase<vMethodPtr, true, true, Class const&&, Ret, Args...> {};

        /// MethodConverter specialization for `Ret(Class::*)(Args..., ...) volatile && noexcept` signature
        template <typename Class, typename Ret, typename... Args, auto vMethodPtr>
        struct MethodConverter<Ret(Class::*)(Args..., ...) volatile && noexcept, vMethodPtr>
            : MethodConverterBase<vMethodPtr, true, true, Class volatile&&, Ret, Args...> {};

        /// MethodConverter specialization for `Ret(Class::*)(Args..., ...) const volatile && noexcept` signature
        template <typename Class, typename Ret, typename... Args, auto vMethodPtr>
        struct MethodConverter<Ret(Class::*)(Args..., ...) const volatile && noexcept, vMethodPtr>
            : MethodConverterBase<vMethodPtr, true, true, Class const volatile&&, Ret, Args...> {};

        /// Convert method pointer to C-like function pointer
        template <typename MP, MP vMethodPtr>
        constexpr auto as_free_function() noexcept
        {
            return MethodConverter<MP, vMethodPtr>::as_free_function();
        }

        /// Convert method pointer to C-like function pointer
        template <auto vMethodPtr>
        constexpr auto as_free_function() noexcept
        {
            return as_free_function<decltype(vMethodPtr), vMethodPtr>();
        }
    }

    /// Enumeration that specifies how compiled code will be outputed
    enum class OutputType : int32_t
    {
        Dll        = TCC_OUTPUT_DLL,
        Executable = TCC_OUTPUT_EXE,
        Object     = TCC_OUTPUT_OBJ
    };

    /// Wrapper around tcc state with set of useful methods
    class TccWrapper
    {
    public:

        using State_t     = TCCState*;
        using ErrorFunc_t = void (*)(void* opaque, char const* msg);

        /// Create wrapper object from (possibly) existing state
        static TccWrapper from(State_t state)
        {
            return TccWrapper{ state };
        }

        /// Deleted named-ctor to prevent construction from nullptr
        static TccWrapper from(std::nullptr_t) = delete;

        #if defined(TW_USE_EXCEPTIONS)

        /// Create wrapper object with valid state or throw on failure
        static TccWrapper with_state()
        {
            if (auto state = tcc_new())
            {
                return TccWrapper{ state };
            }

            throw std::runtime_error("TccWrapper::with_state() - unable to create tcc state");
        }

        #endif

        #if defined(TW_USE_OPTIONAL)

        /// Create optional of wrapper object with valid state or empty optional on failure
        static std::optional<TccWrapper> opt_with_state()
        {
            if (auto state = tcc_new())
            {
                return TccWrapper{ state };
            }

            return std::nullopt;
        }

        #endif

        /// Create invalid (without state) wrapper object
        TccWrapper() noexcept
            : m_state { nullptr }
        {}

        /// Deleted const copy-ctor
        TccWrapper(TccWrapper const&) = delete;

        /// Deleted const copy-assign-op
        TccWrapper& operator=(TccWrapper const&) = delete;

        /// Move-ctor
        TccWrapper(TccWrapper&& other) noexcept
            : m_state { std::exchange(other.m_state, nullptr) }
        {}

        /// Move-assign-op
        TccWrapper& operator=(TccWrapper&& other) noexcept
        {
            if (this != &other)
            {
                if (is_valid())
                {
                    tcc_delete(m_state);
                }

                m_state = std::exchange(other.m_state, nullptr);
            }

            return *this;
        }

        /// Destroy tcc state
        ~TccWrapper() noexcept
        {
            if (is_valid())
            {
                tcc_delete(m_state);
            }
        }

        /// Create (or recreate) tcc state, return true on success
        bool create_state() noexcept
        {
            if (is_valid())
            {
                tcc_delete(m_state);
            }

            m_state = tcc_new();

            return is_valid();
        }

        /// Destroy tcc state, return true if it was deleted
        void destroy_state() noexcept
        {
            if (is_valid())
            {
                tcc_delete(m_state);

                m_state = nullptr;
            }
        }

        /// Set function for printing error messages
        void set_error_callback(void* userData, ErrorFunc_t function) const noexcept
        {
            tcc_set_error_func(m_state, userData, function);
        }

        /// Set options as from command line (like "-std=c99 -O2")
        void set_options(char const* options) const noexcept
        {
            tcc_set_options(m_state, options);
        }

        /// Add include path (as with -Ipath)
        void add_include_path(char const* path) const noexcept
        {
            tcc_add_include_path(m_state, path);
        }

        /// Add system include path (as with -isystem path)
        void add_system_include_path(char const* path) const noexcept
        {
            tcc_add_sysinclude_path(m_state, path);
        }

        /// Add library path (as with -Lpath)
        void add_library_path(char const* path) const noexcept
        {
            tcc_add_library_path(m_state, path);
        }

        /// Add library (as with -lname)
        void add_library(char const* name) const noexcept
        {
            tcc_add_library(m_state, name);
        }

        /// Add file { C file, dll, object, library, ld script } for compilation, return true on success
        bool add_file(char const* path) const noexcept
        {
            return tcc_add_file(m_state, path) != -1;
        }

        /// Add null-terminated string containing C source for compilation, return true on success
        bool add_source_code(char const* src) const noexcept
        {
            return tcc_compile_string(m_state, src) != -1;
        }

        /// Compile code to auto managed memory, return true on success, call only once
        bool compile() const noexcept
        {
            tcc_set_output_type(m_state, TCC_OUTPUT_MEMORY);

            return tcc_relocate(m_state, TCC_RELOCATE_AUTO) != -1;
        }

        /// Define macro with given name and optional value (as with #define name value)
        void define(char const* name, char const* value = nullptr) const noexcept
        {
            tcc_define_symbol(m_state, name, value);
        }

        /// Undefine macro with given name (as with #undef name)
        void undefine(char const* name) const noexcept
        {
            tcc_undefine_symbol(m_state, name);
        }

        /// Add symbol with given name
        void add_symbol(char const* name, void const* symbol) const noexcept
        {
            tcc_add_symbol(m_state, name, symbol);
        }

        /// Return void pointer to symbol with given name or nullptr if no such symbol exists
        void* get_symbol(char const* name) const noexcept
        {
            return tcc_get_symbol(m_state, name);
        }

        /// Return T pointer to symbol with given name or nullptr if no such symbol exists
        template <typename T>
        T* get_symbol_as(char const* name) const noexcept
        {
            return priv::bit_cast<T*>(get_symbol(name));
        }

        /// Check if symbol with given name exists
        bool has_symbol(char const* name) const noexcept
        {
            return get_symbol(name) != nullptr;
        }

        /// Register symbol from parameter as free function with given name, won't override if called multiple times
        template <typename FP>
        void register_function(char const* name, FP functionPtr) const noexcept
        {
            if constexpr (priv::traits::FunctionPtr_v<FP>)
            {
                tcc_add_symbol(m_state, name, priv::bit_cast<void const*>(functionPtr));
            }
            else
            {
                static_assert(priv::error<FP>, "FP is not a function pointer!");
            }
        }

        /// Register symbol as free function with given name, won't override if called multiple times
        template <typename F, F vFunctionPtr>
        void register_function(char const* name) const noexcept
        {
            register_function(name, vFunctionPtr);
        }

        /// Register symbol as free function with given name, won't override if called multiple times
        template <auto vFunctionPtr>
        void register_function(char const* name) const noexcept
        {
            register_function(name, vFunctionPtr);
        }

        /// Register symbol as class method with given name, won't override if called multiple times
        template <typename MP, MP vMethodPtr>
        void register_method(char const* name) const noexcept
        {
            if constexpr (priv::traits::MethodPtr_v<MP>)
            {
                register_function(name, priv::as_free_function<MP, vMethodPtr>());
            }
            else
            {
                static_assert(priv::error<MP>, "MP is not a method pointer!");
            }
        }

        /// Register symbol as class method with given name, won't override if called multiple times
        template <auto vMethodPtr>
        void register_method(char const* name) const noexcept
        {
            register_method<decltype(vMethodPtr), vMethodPtr>(name);
        }

        /// Return F pointer to function with given name or nullptr if no such symbol exists
        template <typename F>
        auto get_function(char const* name) const noexcept
        {
            if constexpr (priv::traits::Function_v<F>)
            {
                return get_symbol_as<F>(name);
            }
            else
            {
                static_assert(priv::error<F>, "F is not a function!");
            }
        }

        #if defined(TW_USE_EXCEPTIONS)

        /// Try to invoke function with given args, return result, throw if no such function symbol exists
        template <typename F, typename... Args>
        auto invoke(char const* name, Args&&... args) const
        {
            if constexpr (priv::traits::Function_v<F>)
            {
                if constexpr (priv::traits::InvokableWith_v<F, Args...>)
                {
                    auto const symbol = get_function<F>(name);

                    if (symbol != nullptr)
                    {
                        return (*symbol)(std::forward<Args>(args)...);
                    }

                    throw std::runtime_error(std::string{ "TccWrapper::invoke() - unable to find symbol with given name: " } + name);
                }
                else
                {
                    static_assert(priv::error<F, Args...>, "F is not invokable with given Args!");
                }
            }
            else
            {
                static_assert(priv::error<F>, "F is not a function!");
            }
        }

        #endif

        #if defined(TW_USE_OPTIONAL)

        /// Try to invoke function with given args, return optional with call result (with value or empty)
        template <typename F, typename... Args>
        auto opt_invoke(char const* name, Args&&... args) const
        {
            if constexpr (priv::traits::Function_v<F>)
            {
                if constexpr (priv::traits::InvokableWith_v<F, Args...>)
                {
                    auto const symbol = get_function<F>(name);

                    return symbol != nullptr ? std::make_optional((*symbol)(std::forward<Args>(args)...)) : std::nullopt;
                }
                else
                {
                    static_assert(priv::error<F, Args...>, "F is not invokable with given Args!");
                }
            }
            else
            {
                static_assert(priv::error<F>, "F is not a function!");
            }
        }

        #endif

        /// Output file depending on outputType, return true on success
        bool output_file(char const* filename, OutputType outputType) const noexcept
        {
            tcc_set_output_type(m_state, static_cast<int32_t>(outputType));

            return tcc_output_file(m_state, filename) != -1;
        }

        /// Return true if internal state is valid (not nullptr)
        bool is_valid() const noexcept
        {
            return m_state != nullptr;
        }

        /// Return internal state on which wrapper operates
        State_t get_state() const noexcept
        {
            return m_state;
        }

    private:

        /// PRIV: Internal private ctor
        TccWrapper(State_t state) noexcept
            : m_state { state }
        {

        }

        State_t m_state;
    };
}

#else

#error TccWrapper requires at least C++17 capable compiler to work

#endif
