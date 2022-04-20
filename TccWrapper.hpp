/*
    Convenient header-only C++20 wrapper to use with embedded Tiny C Compiler (tcc).

    Created by Patrick Stritch
*/

#pragma once

#if (defined(_MSVC_LANG) && _MSVC_LANG >= 202002L) || __cplusplus >= 202002L

// C++
#include <bit>
#include <cstring>
#include <tuple>
#include <optional>
#include <stdexcept>
#include <string>

extern "C"
{
    #define TCC_OUTPUT_MEMORY     1
    #define TCC_OUTPUT_EXE        2
    #define TCC_OUTPUT_DLL        3
    #define TCC_OUTPUT_OBJ        4
    #define TCC_OUTPUT_PREPROCESS 5

    #define TCC_RELOCATE_AUTO     (void*)1

    struct TCCState;

    typedef struct TCCState TCCState;

    typedef void (*TCCErrorFunc)(void* userData, char const* msg);

    typedef void (*TCCListSymbolsFunc)(void* userData, char const* name, void const* value);

    TCCState* tcc_new(void);

    void tcc_delete(TCCState* state);

    void tcc_set_lib_path(TCCState* state, char const* path);

    void tcc_set_error_func(TCCState* state, void* userData, TCCErrorFunc function);

    TCCErrorFunc tcc_get_error_func(TCCState* state);

    void* tcc_get_error_opaque(TCCState* state);

    void tcc_set_options(TCCState* state, char const* options);

    int tcc_add_include_path(TCCState* state, char const* path);

    int tcc_add_sysinclude_path(TCCState* state, char const* path);

    void tcc_define_symbol(TCCState* state, char const* symbol, char const* value);

    void tcc_undefine_symbol(TCCState* state, char const* symbol);

    int tcc_add_file(TCCState* state, char const* filename);

    int tcc_compile_string(TCCState* state, char const* src);

    int tcc_set_output_type(TCCState* state, int outputType);

    int tcc_add_library_path(TCCState* state, char const* path);

    int tcc_add_library(TCCState* state, char const* name);

    int tcc_add_symbol(TCCState* state, char const* name, void const* ptr);

    int tcc_output_file(TCCState* state, char const* filename);

    int tcc_run(TCCState* state, int argc, char** argv);

    int tcc_relocate(TCCState* state, void* ptr);

    void* tcc_get_symbol(TCCState* state, char const* name);

    void tcc_list_symbols(TCCState* state, void* userData, TCCListSymbolsFunc function);
}

namespace tw
{
    namespace priv
    {
        /// Fallback helper for more readable template error messages
        template <typename...>
        inline constexpr bool alwaysFalse = false;

        /// Helper struct holder for AsFreeFunction method
        template <auto vMethodPtr, bool vIsNoexcept, bool vIsCVariadic, typename Class, typename Ret, typename... Args>
        struct MethodConverterBase
        {
            /// Convert method pointer to C-like function pointer
            static constexpr auto AsFreeFunction() noexcept
            {
                if constexpr (vIsCVariadic)
                {
                    static_assert(alwaysFalse<Class>, "C-like variadic arguments in method are not supported!");
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
        struct MethodConverter<Ret(Class::*)(Args...), vMethodPtr> : MethodConverterBase<vMethodPtr, false, false, Class, Ret, Args...> {};

        /// MethodConverter specialization for `Ret(Class::*)(Args...) const` signature
        template <typename Class, typename Ret, typename... Args, auto vMethodPtr>
        struct MethodConverter<Ret(Class::*)(Args...) const, vMethodPtr> : MethodConverterBase<vMethodPtr, false, false, Class const, Ret, Args...> {};

        /// MethodConverter specialization for `Ret(Class::*)(Args...) volatile` signature
        template <typename Class, typename Ret, typename... Args, auto vMethodPtr>
        struct MethodConverter<Ret(Class::*)(Args...) volatile, vMethodPtr> : MethodConverterBase<vMethodPtr, false, false, Class volatile, Ret, Args...> {};

        /// MethodConverter specialization for `Ret(Class::*)(Args...) const volatile` signature
        template <typename Class, typename Ret, typename... Args, auto vMethodPtr>
        struct MethodConverter<Ret(Class::*)(Args...) const volatile, vMethodPtr> : MethodConverterBase<vMethodPtr, false, false, Class const volatile, Ret, Args...> {};

        /// MethodConverter specialization for `Ret(Class::*)(Args..., ...)` signature
        template <typename Class, typename Ret, typename... Args, auto vMethodPtr>
        struct MethodConverter<Ret(Class::*)(Args..., ...), vMethodPtr> : MethodConverterBase<vMethodPtr, false, true, Class, Ret, Args...> {};

        /// MethodConverter specialization for `Ret(Class::*)(Args..., ...) const` signature
        template <typename Class, typename Ret, typename... Args, auto vMethodPtr>
        struct MethodConverter<Ret(Class::*)(Args..., ...) const, vMethodPtr> : MethodConverterBase<vMethodPtr, false, true, Class const, Ret, Args...> {};

        /// MethodConverter specialization for `Ret(Class::*)(Args..., ...) volatile` signature
        template <typename Class, typename Ret, typename... Args, auto vMethodPtr>
        struct MethodConverter<Ret(Class::*)(Args..., ...) volatile, vMethodPtr> : MethodConverterBase<vMethodPtr, false, true, Class volatile, Ret, Args...> {};

        /// MethodConverter specialization for `Ret(Class::*)(Args..., ...) const volatile` signature
        template <typename Class, typename Ret, typename... Args, auto vMethodPtr>
        struct MethodConverter<Ret(Class::*)(Args..., ...) const volatile, vMethodPtr> : MethodConverterBase<vMethodPtr, false, true, Class const volatile, Ret, Args...> {};

        /// MethodConverter specialization for `Ret(Class::*)(Args...) &` signature
        template <typename Class, typename Ret, typename... Args, auto vMethodPtr>
        struct MethodConverter<Ret(Class::*)(Args...) &, vMethodPtr> : MethodConverterBase<vMethodPtr, false, false, Class&, Ret, Args...> {};

        /// MethodConverter specialization for `Ret(Class::*)(Args...) const &` signature
        template <typename Class, typename Ret, typename... Args, auto vMethodPtr>
        struct MethodConverter<Ret(Class::*)(Args...) const &, vMethodPtr> : MethodConverterBase<vMethodPtr, false, false, Class const&, Ret, Args...> {};

        /// MethodConverter specialization for `Ret(Class::*)(Args...) volatile &` signature
        template <typename Class, typename Ret, typename... Args, auto vMethodPtr>
        struct MethodConverter<Ret(Class::*)(Args...) volatile &, vMethodPtr> : MethodConverterBase<vMethodPtr, false, false, Class volatile&, Ret, Args...> {};

        /// MethodConverter specialization for `Ret(Class::*)(Args...) const volatile &` signature
        template <typename Class, typename Ret, typename... Args, auto vMethodPtr>
        struct MethodConverter<Ret(Class::*)(Args...) const volatile &, vMethodPtr> : MethodConverterBase<vMethodPtr, false, false, Class const volatile&, Ret, Args...> {};

        /// MethodConverter specialization for `Ret(Class::*)(Args..., ...) &` signature
        template <typename Class, typename Ret, typename... Args, auto vMethodPtr>
        struct MethodConverter<Ret(Class::*)(Args..., ...) &, vMethodPtr> : MethodConverterBase<vMethodPtr, false, true, Class&, Ret, Args...> {};

        /// MethodConverter specialization for `Ret(Class::*)(Args..., ...) const &` signature
        template <typename Class, typename Ret, typename... Args, auto vMethodPtr>
        struct MethodConverter<Ret(Class::*)(Args..., ...) const &, vMethodPtr> : MethodConverterBase<vMethodPtr, false, true, Class const&, Ret, Args...> {};

        /// MethodConverter specialization for `Ret(Class::*)(Args..., ...) volatile &` signature
        template <typename Class, typename Ret, typename... Args, auto vMethodPtr>
        struct MethodConverter<Ret(Class::*)(Args..., ...) volatile &, vMethodPtr> : MethodConverterBase<vMethodPtr, false, true, Class volatile&, Ret, Args...> {};

        /// MethodConverter specialization for `Ret(Class::*)(Args..., ...) const volatile &` signature
        template <typename Class, typename Ret, typename... Args, auto vMethodPtr>
        struct MethodConverter<Ret(Class::*)(Args..., ...) const volatile &, vMethodPtr> : MethodConverterBase<vMethodPtr, false, true, Class const volatile&, Ret, Args...> {};

        /// MethodConverter specialization for `Ret(Class::*)(Args...) &&` signature
        template <typename Class, typename Ret, typename... Args, auto vMethodPtr>
        struct MethodConverter<Ret(Class::*)(Args...) &&, vMethodPtr> : MethodConverterBase<vMethodPtr, false, false, Class&&, Ret, Args...> {};

        /// MethodConverter specialization for `Ret(Class::*)(Args...) const &&` signature
        template <typename Class, typename Ret, typename... Args, auto vMethodPtr>
        struct MethodConverter<Ret(Class::*)(Args...) const &&, vMethodPtr> : MethodConverterBase<vMethodPtr, false, false, Class const&&, Ret, Args...> {};

        /// MethodConverter specialization for `Ret(Class::*)(Args...) volatile &&` signature
        template <typename Class, typename Ret, typename... Args, auto vMethodPtr>
        struct MethodConverter<Ret(Class::*)(Args...) volatile &&, vMethodPtr> : MethodConverterBase<vMethodPtr, false, false, Class volatile&&, Ret, Args...> {};

        /// MethodConverter specialization for `Ret(Class::*)(Args...) const volatile &&` signature
        template <typename Class, typename Ret, typename... Args, auto vMethodPtr>
        struct MethodConverter<Ret(Class::*)(Args...) const volatile &&, vMethodPtr> : MethodConverterBase<vMethodPtr, false, false, Class const volatile&&, Ret, Args...> {};

        /// MethodConverter specialization for `Ret(Class::*)(Args..., ...) &&` signature
        template <typename Class, typename Ret, typename... Args, auto vMethodPtr>
        struct MethodConverter<Ret(Class::*)(Args..., ...) &&, vMethodPtr> : MethodConverterBase<vMethodPtr, false, true, Class&&, Ret, Args...> {};

        /// MethodConverter specialization for `Ret(Class::*)(Args..., ...) const &&` signature
        template <typename Class, typename Ret, typename... Args, auto vMethodPtr>
        struct MethodConverter<Ret(Class::*)(Args..., ...) const &&, vMethodPtr> : MethodConverterBase<vMethodPtr, false, true, Class const&&, Ret, Args...> {};

        /// MethodConverter specialization for `Ret(Class::*)(Args..., ...) volatile &&` signature
        template <typename Class, typename Ret, typename... Args, auto vMethodPtr>
        struct MethodConverter<Ret(Class::*)(Args..., ...) volatile &&, vMethodPtr> : MethodConverterBase<vMethodPtr, false, true, Class volatile&&, Ret, Args...> {};

        /// MethodConverter specialization for `Ret(Class::*)(Args..., ...) const volatile &&` signature
        template <typename Class, typename Ret, typename... Args, auto vMethodPtr>
        struct MethodConverter<Ret(Class::*)(Args..., ...) const volatile &&, vMethodPtr> : MethodConverterBase<vMethodPtr, false, true, Class const volatile&&, Ret, Args...> {};

        /// MethodConverter specialization for `Ret(Class::*)(Args...) noexcept` signature
        template <typename Class, typename Ret, typename... Args, auto vMethodPtr>
        struct MethodConverter<Ret(Class::*)(Args...) noexcept, vMethodPtr> : MethodConverterBase<vMethodPtr, true, false, Class, Ret, Args...> {};

        /// MethodConverter specialization for `Ret(Class::*)(Args...) const noexcept` signature
        template <typename Class, typename Ret, typename... Args, auto vMethodPtr>
        struct MethodConverter<Ret(Class::*)(Args...) const noexcept, vMethodPtr> : MethodConverterBase<vMethodPtr, true, false, Class const, Ret, Args...> {};

        /// MethodConverter specialization for `Ret(Class::*)(Args...) volatile noexcept` signature
        template <typename Class, typename Ret, typename... Args, auto vMethodPtr>
        struct MethodConverter<Ret(Class::*)(Args...) volatile noexcept, vMethodPtr> : MethodConverterBase<vMethodPtr, true, false, Class volatile, Ret, Args...> {};

        /// MethodConverter specialization for `Ret(Class::*)(Args...) const volatile noexcept` signature
        template <typename Class, typename Ret, typename... Args, auto vMethodPtr>
        struct MethodConverter<Ret(Class::*)(Args...) const volatile noexcept, vMethodPtr> : MethodConverterBase<vMethodPtr, true, false, Class const volatile, Ret, Args...> {};

        /// MethodConverter specialization for `Ret(Class::*)(Args..., ...) noexcept` signature
        template <typename Class, typename Ret, typename... Args, auto vMethodPtr>
        struct MethodConverter<Ret(Class::*)(Args..., ...) noexcept, vMethodPtr> : MethodConverterBase<vMethodPtr, true, true, Class, Ret, Args...> {};

        /// MethodConverter specialization for `Ret(Class::*)(Args..., ...) const noexcept` signature
        template <typename Class, typename Ret, typename... Args, auto vMethodPtr>
        struct MethodConverter<Ret(Class::*)(Args..., ...) const noexcept, vMethodPtr> : MethodConverterBase<vMethodPtr, true, true, Class const, Ret, Args...> {};

        /// MethodConverter specialization for `Ret(Class::*)(Args..., ...) volatile noexcept` signature
        template <typename Class, typename Ret, typename... Args, auto vMethodPtr>
        struct MethodConverter<Ret(Class::*)(Args..., ...) volatile noexcept, vMethodPtr> : MethodConverterBase<vMethodPtr, true, true, Class volatile, Ret, Args...> {};

        /// MethodConverter specialization for `Ret(Class::*)(Args..., ...) const volatile noexcept` signature
        template <typename Class, typename Ret, typename... Args, auto vMethodPtr>
        struct MethodConverter<Ret(Class::*)(Args..., ...) const volatile noexcept, vMethodPtr> : MethodConverterBase<vMethodPtr, true, true, Class const volatile, Ret, Args...> {};

        /// MethodConverter specialization for `Ret(Class::*)(Args...) & noexcept` signature
        template <typename Class, typename Ret, typename... Args, auto vMethodPtr>
        struct MethodConverter<Ret(Class::*)(Args...) & noexcept, vMethodPtr> : MethodConverterBase<vMethodPtr, true, false, Class&, Ret, Args...> {};

        /// MethodConverter specialization for `Ret(Class::*)(Args...) const & noexcept` signature
        template <typename Class, typename Ret, typename... Args, auto vMethodPtr>
        struct MethodConverter<Ret(Class::*)(Args...) const & noexcept, vMethodPtr> : MethodConverterBase<vMethodPtr, true, false, Class const&, Ret, Args...> {};

        /// MethodConverter specialization for `Ret(Class::*)(Args...) volatile & noexcept` signature
        template <typename Class, typename Ret, typename... Args, auto vMethodPtr>
        struct MethodConverter<Ret(Class::*)(Args...) volatile & noexcept, vMethodPtr> : MethodConverterBase<vMethodPtr, true, false, Class volatile&, Ret, Args...> {};

        /// MethodConverter specialization for `Ret(Class::*)(Args...) const volatile & noexcept` signature
        template <typename Class, typename Ret, typename... Args, auto vMethodPtr>
        struct MethodConverter<Ret(Class::*)(Args...) const volatile & noexcept, vMethodPtr> : MethodConverterBase<vMethodPtr, true, false, Class const volatile&, Ret, Args...> {};

        /// MethodConverter specialization for `Ret(Class::*)(Args..., ...) & noexcept` signature
        template <typename Class, typename Ret, typename... Args, auto vMethodPtr>
        struct MethodConverter<Ret(Class::*)(Args..., ...) & noexcept, vMethodPtr> : MethodConverterBase<vMethodPtr, true, true, Class&, Ret, Args...> {};

        /// MethodConverter specialization for `Ret(Class::*)(Args..., ...) const & noexcept` signature
        template <typename Class, typename Ret, typename... Args, auto vMethodPtr>
        struct MethodConverter<Ret(Class::*)(Args..., ...) const & noexcept, vMethodPtr> : MethodConverterBase<vMethodPtr, true, true, Class const&, Ret, Args...> {};

        /// MethodConverter specialization for `Ret(Class::*)(Args..., ...) volatile & noexcept` signature
        template <typename Class, typename Ret, typename... Args, auto vMethodPtr>
        struct MethodConverter<Ret(Class::*)(Args..., ...) volatile & noexcept, vMethodPtr> : MethodConverterBase<vMethodPtr, true, true, Class volatile&, Ret, Args...> {};

        /// MethodConverter specialization for `Ret(Class::*)(Args..., ...) const volatile & noexcept` signature
        template <typename Class, typename Ret, typename... Args, auto vMethodPtr>
        struct MethodConverter<Ret(Class::*)(Args..., ...) const volatile & noexcept, vMethodPtr> : MethodConverterBase<vMethodPtr, true, true, Class const volatile&, Ret, Args...> {};

        /// MethodConverter specialization for `Ret(Class::*)(Args...) && noexcept` signature
        template <typename Class, typename Ret, typename... Args, auto vMethodPtr>
        struct MethodConverter<Ret(Class::*)(Args...) && noexcept, vMethodPtr> : MethodConverterBase<vMethodPtr, true, false, Class&&, Ret, Args...> {};

        /// MethodConverter specialization for `Ret(Class::*)(Args...) const && noexcept` signature
        template <typename Class, typename Ret, typename... Args, auto vMethodPtr>
        struct MethodConverter<Ret(Class::*)(Args...) const && noexcept, vMethodPtr> : MethodConverterBase<vMethodPtr, true, false, Class const&&, Ret, Args...> {};

        /// MethodConverter specialization for `Ret(Class::*)(Args...) volatile && noexcept` signature
        template <typename Class, typename Ret, typename... Args, auto vMethodPtr>
        struct MethodConverter<Ret(Class::*)(Args...) volatile && noexcept, vMethodPtr> : MethodConverterBase<vMethodPtr, true, false, Class volatile&&, Ret, Args...> {};

        /// MethodConverter specialization for `Ret(Class::*)(Args...) const volatile && noexcept` signature
        template <typename Class, typename Ret, typename... Args, auto vMethodPtr>
        struct MethodConverter<Ret(Class::*)(Args...) const volatile && noexcept, vMethodPtr> : MethodConverterBase<vMethodPtr, true, false, Class const volatile&&, Ret, Args...> {};

        /// MethodConverter specialization for `Ret(Class::*)(Args..., ...) && noexcept` signature
        template <typename Class, typename Ret, typename... Args, auto vMethodPtr>
        struct MethodConverter<Ret(Class::*)(Args..., ...) && noexcept, vMethodPtr> : MethodConverterBase<vMethodPtr, true, true, Class&&, Ret, Args...> {};

        /// MethodConverter specialization for `Ret(Class::*)(Args..., ...) const && noexcept` signature
        template <typename Class, typename Ret, typename... Args, auto vMethodPtr>
        struct MethodConverter<Ret(Class::*)(Args..., ...) const && noexcept, vMethodPtr> : MethodConverterBase<vMethodPtr, true, true, Class const&&, Ret, Args...> {};

        /// MethodConverter specialization for `Ret(Class::*)(Args..., ...) volatile && noexcept` signature
        template <typename Class, typename Ret, typename... Args, auto vMethodPtr>
        struct MethodConverter<Ret(Class::*)(Args..., ...) volatile && noexcept, vMethodPtr> : MethodConverterBase<vMethodPtr, true, true, Class volatile&&, Ret, Args...> {};

        /// MethodConverter specialization for `Ret(Class::*)(Args..., ...) const volatile && noexcept` signature
        template <typename Class, typename Ret, typename... Args, auto vMethodPtr>
        struct MethodConverter<Ret(Class::*)(Args..., ...) const volatile && noexcept, vMethodPtr> : MethodConverterBase<vMethodPtr, true, true, Class const volatile&&, Ret, Args...> {};

        namespace cep
        {
            template <typename T>
            concept Function = std::is_function_v<T>;

            template <typename T>
            concept FunctionPtr = Function<std::remove_pointer_t<T>>;

            template <typename T>
            concept MethodPtr = std::is_member_function_pointer_v<T>;

            template <typename T, typename... Ts>
            concept InvokableWith = Function<T> && std::is_invocable_v<T, Ts...>;

            template <typename U, typename T, typename... Ts>
            concept InvokableToWith = InvokableWith<T, Ts...> && std::is_assignable_v<U, std::invoke_result_t<T, Ts...>>;
        }
    }

    /// Convert method pointer to C-like function pointer
    template <typename M, M vMethodPtr>
        requires priv::cep::MethodPtr<M>
    constexpr auto AsFreeFunction() noexcept
    {
        return priv::MethodConverter<M, vMethodPtr>::AsFreeFunction();
    }

    /// Convert method pointer to C-like function pointer
    template <auto vMethodPtr>
        requires priv::cep::MethodPtr<decltype(vMethodPtr)>
    constexpr auto AsFreeFunction() noexcept
    {
        return AsFreeFunction<decltype(vMethodPtr), vMethodPtr>();
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

        using State_t           = TCCState*;
        using ErrorFunc_t       = TCCErrorFunc;
        using ListSymbolsFunc_t = TCCListSymbolsFunc;

        /// Create invalid (without state) wrapper object
        static TccWrapper New() noexcept
        {
            return TccWrapper{ nullptr };
        }

        /// Create wrapper object from existing state
        static TccWrapper From(State_t state)
        {
            return TccWrapper{ state };
        }

        /// Deleted named-ctor to prevent construction from nullptr
        static TccWrapper From(std::nullptr_t) = delete;

        /// Create wrapper object with valid state or throw on failure
        static TccWrapper WithState()
        {
            auto state = tcc_new();

            if (state)
            {
                return TccWrapper{ state };
            }
            else
            {
                throw std::runtime_error("TccWrapper::WithState() - unable to create tcc state");
            }
        }

        /// Create optional of wrapper object with valid state or empty optional on failure
        static std::optional<TccWrapper> OptWithState()
        {
            auto state = tcc_new();

            if (state)
            {
                return TccWrapper{ state };
            }
            else
            {
                return std::nullopt;
            }
        }

        /// Deleted const copy-ctor
        TccWrapper(TccWrapper const&) = delete;

        /// Deleted const copy-assign-op
        TccWrapper& operator=(TccWrapper const&) = delete;

        /// Move-ctor
        TccWrapper(TccWrapper&& rhs) noexcept
            : m_state { rhs.m_state }
        {
            rhs.m_state = nullptr;
        }

        /// Move-assign-op
        TccWrapper& operator=(TccWrapper&& rhs) noexcept
        {
            if (this != &rhs)
            {
                if (m_state)
                {
                    tcc_delete(m_state);
                }

                m_state = rhs.m_state;

                rhs.m_state = nullptr;
            }

            return *this;
        }

        /// Deleted const move-ctor to prevent moving from const
        TccWrapper(TccWrapper const&&) = delete;

        /// Deleted const move-assign-op to prevent moving from const
        TccWrapper& operator=(TccWrapper const&&) = delete;

        /// Destroy tcc state
        ~TccWrapper() noexcept
        {
            if (m_state)
            {
                tcc_delete(m_state);
            }
        }

        /// Create tcc state, return true on success
        bool CreateState() noexcept
        {
            if (m_state)
            {
                tcc_delete(m_state);
            }

            m_state = tcc_new();

            return m_state;
        }

        /// Destroy tcc state, return true if it was deleted
        bool DestroyState() noexcept
        {
            if (m_state)
            {
                tcc_delete(m_state);

                m_state = nullptr;

                return true;
            }

            return false;
        }

        /// Set function for printing error messages
        void SetErrorCallback(void* userData, ErrorFunc_t function) const noexcept
        {
            tcc_set_error_func(m_state, userData, function);
        }

        /// Set options as from command line (like "-std=c99 -O2")
        void SetOptions(char const* options) const noexcept
        {
            tcc_set_options(m_state, options);
        }

        /// Add include path (as with -Ipath)
        void AddIncludePath(char const* path) const noexcept
        {
            tcc_add_include_path(m_state, path);
        }

        /// Add system include path (as with -isystem path)
        void AddSystemIncludePath(char const* path) const noexcept
        {
            tcc_add_sysinclude_path(m_state, path);
        }

        /// Add library path (as with -Lpath)
        void AddLibraryPath(char const* path) const noexcept
        {
            tcc_add_library_path(m_state, path);
        }

        /// Add library (as with -lname)
        void AddLibrary(char const* name) const noexcept
        {
            tcc_add_library(m_state, name);
        }

        /// Add file { C file, dll, object, library, ld script } for compilation, return true on success
        bool AddFile(char const* path) const noexcept
        {
            return tcc_add_file(m_state, path) != -1;
        }

        /// Add null-terminated string containing C source for compilation, return true on success
        bool AddSourceCode(char const* src) const noexcept
        {
            return tcc_compile_string(m_state, src) != -1;
        }

        /// Compile code to auto managed memory (via tcc_mallocz), return true on success, call only once
        bool Compile() const noexcept
        {
            tcc_set_output_type(m_state, TCC_OUTPUT_MEMORY);

            return tcc_relocate(m_state, TCC_RELOCATE_AUTO) != -1;
        }

        /// Return required size for the buffer to which code will be compiled or -1 on failure, call only once
        int32_t GetCodeSize() const noexcept
        {
            tcc_set_output_type(m_state, TCC_OUTPUT_MEMORY);

            return tcc_relocate(m_state, nullptr);
        }

        /// Compile code to user managed buffer, return true on success, call TccWrapper::GetCodeSize before this
        bool Compile(void* buffer) const noexcept
        {
            return tcc_relocate(m_state, buffer) != -1;
        }

        /// Define macro with given name and optional value (as with #define name value)
        void Define(char const* name, char const* value = nullptr) const noexcept
        {
            tcc_define_symbol(m_state, name, value);
        }

        /// Undefine macro with given name (as with #undef name)
        void Undefine(char const* name) const noexcept
        {
            tcc_undefine_symbol(m_state, name);
        }

        /// Add symbol with given name
        void AddSymbol(char const* name, void const* symbol) const noexcept
        {
            tcc_add_symbol(m_state, name, symbol);
        }

        /// Return void pointer to symbol with given name or nullptr if no such symbol exists
        void* GetSymbol(char const* name) const noexcept
        {
            return tcc_get_symbol(m_state, name);
        }

        /// Return T pointer to symbol with given name or nullptr if no such symbol exists
        template <typename T>
        T* GetSymbolAs(char const* name) const noexcept
        {
            return std::bit_cast<T*>(GetSymbol(name));
        }

        /// Check whether symbol with given name exists
        bool HasSymbol(char const* name) const noexcept
        {
            return GetSymbol(name) != nullptr;
        }

        /// Invoke function for each symbol in the state
        void ForEachSymbol(void* userData, ListSymbolsFunc_t function) const noexcept
        {
            tcc_list_symbols(m_state, userData, function);
        }

        /// Register symbol from parameter as free function with given name, won't override if called multiple times
        template <typename F>
            requires priv::cep::Function<F>
        void RegisterFunction(char const* name, F* functionPtr) const noexcept
        {
            tcc_add_symbol(m_state, name, std::bit_cast<void const*>(functionPtr));
        }

        /// Register symbol as free function with given name, won't override if called multiple times
        template <typename F, F vFunctionPtr>
            requires priv::cep::FunctionPtr<F>
        void RegisterFunction(char const* name) const noexcept
        {
            RegisterFunction(name, vFunctionPtr);
        }

        /// Register symbol as free function with given name, won't override if called multiple times
        template <auto vFunctionPtr>
            requires priv::cep::FunctionPtr<decltype(vFunctionPtr)>
        void RegisterFunction(char const* name) const noexcept
        {
            RegisterFunction(name, vFunctionPtr);
        }

        /// Register symbol as class method with given name, won't override if called multiple times
        template <typename M, M vMethodPtr>
            requires priv::cep::MethodPtr<M>
        void RegisterMethod(char const* name) const noexcept
        {
            RegisterFunction(name, AsFreeFunction<M, vMethodPtr>());
        }

        /// Register symbol as class method with given name, won't override if called multiple times
        template <auto vMethodPtr>
            requires priv::cep::MethodPtr<decltype(vMethodPtr)>
        void RegisterMethod(char const* name) const noexcept
        {
            RegisterMethod<decltype(vMethodPtr), vMethodPtr>(name);
        }

        /// Return F pointer to function with given name or nullptr if no such symbol exists
        template <typename F>
            requires priv::cep::Function<F>
        auto GetFunction(char const* name) const noexcept
        {
            return GetSymbolAs<F>(name);
        }

        /// Try to invoke function with given args, return result, throw if no such function symbol exists
        template <typename F, typename... Args>
            requires priv::cep::InvokableWith<F, Args...>
        std::invoke_result_t<F, Args...> Invoke(char const* name, Args&&... args) const
        {
            auto const symbol = GetFunction<F>(name);

            if (symbol)
            {
                return (*symbol)(std::forward<Args>(args)...);
            }

            throw std::runtime_error(std::string{ "TccWrapper::Invoke() - unable to find symbol with given name: " } + name);
        }

        /// Try to invoke function with given args, return optional with invoke result
        template <typename F, typename... Args>
            requires priv::cep::InvokableWith<F, Args...>
        std::optional<std::invoke_result_t<F, Args...>> OptInvoke(char const* name, Args&&... args) const
        {
            auto const symbol = GetFunction<F>(name);

            if (symbol)
            {
                return (*symbol)(std::forward<Args>(args)...);
            }

            return std::nullopt;
        }

        /// Try to invoke function with given args, store result in output argument, return true on success
        template <typename F, typename R, typename... Args>
            requires priv::cep::InvokableToWith<R&, F, Args...>
        bool SafeInvoke(char const* name, R& result, Args&&... args) const
        {
            auto const symbol = GetFunction<F>(name);

            if (symbol)
            {
                result = (*symbol)(std::forward<Args>(args)...);

                return true;
            }

            return false;
        }

        /// Output file depending on outputType, return true on success
        bool OutputFile(char const* filename, OutputType outputType) const noexcept
        {
            tcc_set_output_type(m_state, static_cast<int32_t>(outputType));

            return tcc_output_file(m_state, filename) != -1;
        }

        /// Return internal state on which wrapper operates
        State_t GetState() const noexcept
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

#error TccWrapper requires at least C++20 capable compiler to work

#endif
