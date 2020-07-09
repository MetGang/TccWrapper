/*

    Convenient header-only C++ wrapper for use with embedded Tiny C Compiler

    Created by Patrick Stritch

    Macros:
    * TW_USE_OPTIONAL
    * TW_USE_EXCEPTIONS

*/

#pragma once

// C++
#include <cstring>
#include <tuple>
#ifdef TW_USE_OPTIONAL
#include <optional>
#endif // TW_USE_OPTIONAL
#ifdef TW_USE_EXCEPTIONS
#include <stdexcept>
#include <string>
#endif // TW_USE_EXCEPTIONS

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

    void tcc_set_options(TCCState* state, char const* str);

    int tcc_add_include_path(TCCState* state, char const* path);

    int tcc_add_sysinclude_path(TCCState* state, char const* path);

    void tcc_define_symbol(TCCState* state, char const* symbol, char const* value);

    void tcc_undefine_symbol(TCCState* state, char const* symbol);

    int tcc_add_file(TCCState* state, char const* filename);

    int tcc_compile_string(TCCState* state, char const* str);

    int tcc_set_output_type(TCCState* state, int type);

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
    namespace detail
    {
        ///
        template <typename...>
        inline constexpr bool alwaysFalse = false;

        ///
        template <typename To, typename From>
        constexpr To BitCast(From const& src) noexcept
        {
            if constexpr ((sizeof(To) == sizeof(From)) && std::is_trivially_copyable_v<From> && std::is_trivial_v<To>)
            {
                To dst;
                std::memcpy(&dst, &src, sizeof(To));
                return dst;
            }
            else
            {
                static_assert(detail::alwaysFalse<To, From>, "Type requirements not met!");
            }
        }

        ///
        template <auto vMethodPtr, bool vIsNoexcept, bool vIsCVariadic, typename Class, typename Ret, typename... Args>
        struct MethodConverterBase
        {
            static constexpr auto AsFreeFunction() noexcept
            {
                if constexpr (vIsCVariadic)
                {
                    static_assert(detail::alwaysFalse<Class>, "C-like variadic arguments in method are not supported!");
                }
                else
                {
                    return +[](std::remove_reference_t<Class>* ptr, Args... args) noexcept(vIsNoexcept) -> Ret {
                        return (std::forward<Class>(*ptr).*vMethodPtr)(std::forward<Args>(args)...);
                    };
                }
            }
        };

        ///
        template <typename, auto>
        struct MethodConverter;

        ///
        template <typename Class, typename Ret, typename... Args, auto vMethodPtr>
        struct MethodConverter<Ret(Class::*)(Args...), vMethodPtr> : MethodConverterBase<vMethodPtr, false, false, Class, Ret, Args...> {};

        ///
        template <typename Class, typename Ret, typename... Args, auto vMethodPtr>
        struct MethodConverter<Ret(Class::*)(Args...) const, vMethodPtr> : MethodConverterBase<vMethodPtr, false, false, Class const, Ret, Args...> {};

        ///
        template <typename Class, typename Ret, typename... Args, auto vMethodPtr>
        struct MethodConverter<Ret(Class::*)(Args...) volatile, vMethodPtr> : MethodConverterBase<vMethodPtr, false, false, Class volatile, Ret, Args...> {};

        ///
        template <typename Class, typename Ret, typename... Args, auto vMethodPtr>
        struct MethodConverter<Ret(Class::*)(Args...) const volatile, vMethodPtr> : MethodConverterBase<vMethodPtr, false, false, Class const volatile, Ret, Args...> {};

        ///
        template <typename Class, typename Ret, typename... Args, auto vMethodPtr>
        struct MethodConverter<Ret(Class::*)(Args..., ...), vMethodPtr> : MethodConverterBase<vMethodPtr, false, true, Class, Ret, Args...> {};

        ///
        template <typename Class, typename Ret, typename... Args, auto vMethodPtr>
        struct MethodConverter<Ret(Class::*)(Args..., ...) const, vMethodPtr> : MethodConverterBase<vMethodPtr, false, true, Class const, Ret, Args...> {};

        ///
        template <typename Class, typename Ret, typename... Args, auto vMethodPtr>
        struct MethodConverter<Ret(Class::*)(Args..., ...) volatile, vMethodPtr> : MethodConverterBase<vMethodPtr, false, true, Class volatile, Ret, Args...> {};

        ///
        template <typename Class, typename Ret, typename... Args, auto vMethodPtr>
        struct MethodConverter<Ret(Class::*)(Args..., ...) const volatile, vMethodPtr> : MethodConverterBase<vMethodPtr, false, true, Class const volatile, Ret, Args...> {};

        ///
        template <typename Class, typename Ret, typename... Args, auto vMethodPtr>
        struct MethodConverter<Ret(Class::*)(Args...) &, vMethodPtr> : MethodConverterBase<vMethodPtr, false, false, Class&, Ret, Args...> {};

        ///
        template <typename Class, typename Ret, typename... Args, auto vMethodPtr>
        struct MethodConverter<Ret(Class::*)(Args...) const &, vMethodPtr> : MethodConverterBase<vMethodPtr, false, false, Class const&, Ret, Args...> {};

        ///
        template <typename Class, typename Ret, typename... Args, auto vMethodPtr>
        struct MethodConverter<Ret(Class::*)(Args...) volatile &, vMethodPtr> : MethodConverterBase<vMethodPtr, false, false, Class volatile&, Ret, Args...> {};

        ///
        template <typename Class, typename Ret, typename... Args, auto vMethodPtr>
        struct MethodConverter<Ret(Class::*)(Args...) const volatile &, vMethodPtr> : MethodConverterBase<vMethodPtr, false, false, Class const volatile&, Ret, Args...> {};

        ///
        template <typename Class, typename Ret, typename... Args, auto vMethodPtr>
        struct MethodConverter<Ret(Class::*)(Args..., ...) &, vMethodPtr> : MethodConverterBase<vMethodPtr, false, true, Class&, Ret, Args...> {};

        ///
        template <typename Class, typename Ret, typename... Args, auto vMethodPtr>
        struct MethodConverter<Ret(Class::*)(Args..., ...) const &, vMethodPtr> : MethodConverterBase<vMethodPtr, false, true, Class const&, Ret, Args...> {};

        ///
        template <typename Class, typename Ret, typename... Args, auto vMethodPtr>
        struct MethodConverter<Ret(Class::*)(Args..., ...) volatile &, vMethodPtr> : MethodConverterBase<vMethodPtr, false, true, Class volatile&, Ret, Args...> {};

        ///
        template <typename Class, typename Ret, typename... Args, auto vMethodPtr>
        struct MethodConverter<Ret(Class::*)(Args..., ...) const volatile &, vMethodPtr> : MethodConverterBase<vMethodPtr, false, true, Class const volatile&, Ret, Args...> {};

        ///
        template <typename Class, typename Ret, typename... Args, auto vMethodPtr>
        struct MethodConverter<Ret(Class::*)(Args...) &&, vMethodPtr> : MethodConverterBase<vMethodPtr, false, false, Class&&, Ret, Args...> {};

        ///
        template <typename Class, typename Ret, typename... Args, auto vMethodPtr>
        struct MethodConverter<Ret(Class::*)(Args...) const &&, vMethodPtr> : MethodConverterBase<vMethodPtr, false, false, Class const&&, Ret, Args...> {};

        ///
        template <typename Class, typename Ret, typename... Args, auto vMethodPtr>
        struct MethodConverter<Ret(Class::*)(Args...) volatile &&, vMethodPtr> : MethodConverterBase<vMethodPtr, false, false, Class volatile&&, Ret, Args...> {};

        ///
        template <typename Class, typename Ret, typename... Args, auto vMethodPtr>
        struct MethodConverter<Ret(Class::*)(Args...) const volatile &&, vMethodPtr> : MethodConverterBase<vMethodPtr, false, false, Class const volatile&&, Ret, Args...> {};

        ///
        template <typename Class, typename Ret, typename... Args, auto vMethodPtr>
        struct MethodConverter<Ret(Class::*)(Args..., ...) &&, vMethodPtr> : MethodConverterBase<vMethodPtr, false, true, Class&&, Ret, Args...> {};

        ///
        template <typename Class, typename Ret, typename... Args, auto vMethodPtr>
        struct MethodConverter<Ret(Class::*)(Args..., ...) const &&, vMethodPtr> : MethodConverterBase<vMethodPtr, false, true, Class const&&, Ret, Args...> {};

        ///
        template <typename Class, typename Ret, typename... Args, auto vMethodPtr>
        struct MethodConverter<Ret(Class::*)(Args..., ...) volatile &&, vMethodPtr> : MethodConverterBase<vMethodPtr, false, true, Class volatile&&, Ret, Args...> {};

        ///
        template <typename Class, typename Ret, typename... Args, auto vMethodPtr>
        struct MethodConverter<Ret(Class::*)(Args..., ...) const volatile &&, vMethodPtr> : MethodConverterBase<vMethodPtr, false, true, Class const volatile&&, Ret, Args...> {};

        ///
        template <typename Class, typename Ret, typename... Args, auto vMethodPtr>
        struct MethodConverter<Ret(Class::*)(Args...) noexcept, vMethodPtr> : MethodConverterBase<vMethodPtr, true, false, Class, Ret, Args...> {};

        ///
        template <typename Class, typename Ret, typename... Args, auto vMethodPtr>
        struct MethodConverter<Ret(Class::*)(Args...) const noexcept, vMethodPtr> : MethodConverterBase<vMethodPtr, true, false, Class const, Ret, Args...> {};

        ///
        template <typename Class, typename Ret, typename... Args, auto vMethodPtr>
        struct MethodConverter<Ret(Class::*)(Args...) volatile noexcept, vMethodPtr> : MethodConverterBase<vMethodPtr, true, false, Class volatile, Ret, Args...> {};

        ///
        template <typename Class, typename Ret, typename... Args, auto vMethodPtr>
        struct MethodConverter<Ret(Class::*)(Args...) const volatile noexcept, vMethodPtr> : MethodConverterBase<vMethodPtr, true, false, Class const volatile, Ret, Args...> {};

        ///
        template <typename Class, typename Ret, typename... Args, auto vMethodPtr>
        struct MethodConverter<Ret(Class::*)(Args..., ...) noexcept, vMethodPtr> : MethodConverterBase<vMethodPtr, true, true, Class, Ret, Args...> {};

        ///
        template <typename Class, typename Ret, typename... Args, auto vMethodPtr>
        struct MethodConverter<Ret(Class::*)(Args..., ...) const noexcept, vMethodPtr> : MethodConverterBase<vMethodPtr, true, true, Class const, Ret, Args...> {};

        ///
        template <typename Class, typename Ret, typename... Args, auto vMethodPtr>
        struct MethodConverter<Ret(Class::*)(Args..., ...) volatile noexcept, vMethodPtr> : MethodConverterBase<vMethodPtr, true, true, Class volatile, Ret, Args...> {};

        ///
        template <typename Class, typename Ret, typename... Args, auto vMethodPtr>
        struct MethodConverter<Ret(Class::*)(Args..., ...) const volatile noexcept, vMethodPtr> : MethodConverterBase<vMethodPtr, true, true, Class const volatile, Ret, Args...> {};

        ///
        template <typename Class, typename Ret, typename... Args, auto vMethodPtr>
        struct MethodConverter<Ret(Class::*)(Args...) & noexcept, vMethodPtr> : MethodConverterBase<vMethodPtr, true, false, Class&, Ret, Args...> {};

        ///
        template <typename Class, typename Ret, typename... Args, auto vMethodPtr>
        struct MethodConverter<Ret(Class::*)(Args...) const & noexcept, vMethodPtr> : MethodConverterBase<vMethodPtr, true, false, Class const&, Ret, Args...> {};

        ///
        template <typename Class, typename Ret, typename... Args, auto vMethodPtr>
        struct MethodConverter<Ret(Class::*)(Args...) volatile & noexcept, vMethodPtr> : MethodConverterBase<vMethodPtr, true, false, Class volatile&, Ret, Args...> {};

        ///
        template <typename Class, typename Ret, typename... Args, auto vMethodPtr>
        struct MethodConverter<Ret(Class::*)(Args...) const volatile & noexcept, vMethodPtr> : MethodConverterBase<vMethodPtr, true, false, Class const volatile&, Ret, Args...> {};

        ///
        template <typename Class, typename Ret, typename... Args, auto vMethodPtr>
        struct MethodConverter<Ret(Class::*)(Args..., ...) & noexcept, vMethodPtr> : MethodConverterBase<vMethodPtr, true, true, Class&, Ret, Args...> {};

        ///
        template <typename Class, typename Ret, typename... Args, auto vMethodPtr>
        struct MethodConverter<Ret(Class::*)(Args..., ...) const & noexcept, vMethodPtr> : MethodConverterBase<vMethodPtr, true, true, Class const&, Ret, Args...> {};

        ///
        template <typename Class, typename Ret, typename... Args, auto vMethodPtr>
        struct MethodConverter<Ret(Class::*)(Args..., ...) volatile & noexcept, vMethodPtr> : MethodConverterBase<vMethodPtr, true, true, Class volatile&, Ret, Args...> {};

        ///
        template <typename Class, typename Ret, typename... Args, auto vMethodPtr>
        struct MethodConverter<Ret(Class::*)(Args..., ...) const volatile & noexcept, vMethodPtr> : MethodConverterBase<vMethodPtr, true, true, Class const volatile&, Ret, Args...> {};

        ///
        template <typename Class, typename Ret, typename... Args, auto vMethodPtr>
        struct MethodConverter<Ret(Class::*)(Args...) && noexcept, vMethodPtr> : MethodConverterBase<vMethodPtr, true, false, Class&&, Ret, Args...> {};

        ///
        template <typename Class, typename Ret, typename... Args, auto vMethodPtr>
        struct MethodConverter<Ret(Class::*)(Args...) const && noexcept, vMethodPtr> : MethodConverterBase<vMethodPtr, true, false, Class const&&, Ret, Args...> {};

        ///
        template <typename Class, typename Ret, typename... Args, auto vMethodPtr>
        struct MethodConverter<Ret(Class::*)(Args...) volatile && noexcept, vMethodPtr> : MethodConverterBase<vMethodPtr, true, false, Class volatile&&, Ret, Args...> {};

        ///
        template <typename Class, typename Ret, typename... Args, auto vMethodPtr>
        struct MethodConverter<Ret(Class::*)(Args...) const volatile && noexcept, vMethodPtr> : MethodConverterBase<vMethodPtr, true, false, Class const volatile&&, Ret, Args...> {};

        ///
        template <typename Class, typename Ret, typename... Args, auto vMethodPtr>
        struct MethodConverter<Ret(Class::*)(Args..., ...) && noexcept, vMethodPtr> : MethodConverterBase<vMethodPtr, true, true, Class&&, Ret, Args...> {};

        ///
        template <typename Class, typename Ret, typename... Args, auto vMethodPtr>
        struct MethodConverter<Ret(Class::*)(Args..., ...) const && noexcept, vMethodPtr> : MethodConverterBase<vMethodPtr, true, true, Class const&&, Ret, Args...> {};

        ///
        template <typename Class, typename Ret, typename... Args, auto vMethodPtr>
        struct MethodConverter<Ret(Class::*)(Args..., ...) volatile && noexcept, vMethodPtr> : MethodConverterBase<vMethodPtr, true, true, Class volatile&&, Ret, Args...> {};

        ///
        template <typename Class, typename Ret, typename... Args, auto vMethodPtr>
        struct MethodConverter<Ret(Class::*)(Args..., ...) const volatile && noexcept, vMethodPtr> : MethodConverterBase<vMethodPtr, true, true, Class const volatile&&, Ret, Args...> {};

        ///
        template <typename M, M vMethodPtr>
        constexpr auto AsFreeFunction() noexcept
        {
            return MethodConverter<M, vMethodPtr>::AsFreeFunction();
        }

        ///
        template <auto vMethodPtr>
        constexpr auto AsFreeFunction() noexcept
        {
            return AsFreeFunction<decltype(vMethodPtr), vMethodPtr>();
        }
    }

    /// Enumeration that specifies how code will be compiled
    enum class OutputType : int32_t
    {
        Dll = TCC_OUTPUT_DLL,
        Executable = TCC_OUTPUT_EXE,
        Object = TCC_OUTPUT_OBJ
    };

    /// Returns value as underlying type of OutputType enum
    constexpr auto operator+(OutputType value) noexcept
    {
        return static_cast<std::underlying_type_t<OutputType>>(value);
    }

    /// Wrapper around tcc state with set of useful methods
    class TccWrapper
    {
    public:

        using State_t           = TCCState*;
        using ErrorFunc_t       = TCCErrorFunc;
        using ListSymbolsFunc_t = TCCListSymbolsFunc;

        /// Initializes tcc state to nullptr
        TccWrapper() noexcept
            : m_state { nullptr }
        {

        }

        /// Deleted copy-ctor
        TccWrapper(TccWrapper const&) = delete;

        /// Deleted copy-assign-op
        TccWrapper& operator = (TccWrapper const&) = delete;

        /// Moves state from the rhs
        TccWrapper(TccWrapper&& rhs) noexcept
            : m_state { rhs.m_state }
        {
            rhs.M_Reset();
        }

        /// Moves state from the rhs and deletes current one
        TccWrapper& operator = (TccWrapper&& rhs) noexcept
        {
            if (this != &rhs)
            {
                M_Destroy();

                m_state = rhs.m_state;

                rhs.M_Reset();
            }

            return *this;
        }

        /// Deleted const move-ctor to prevent move from const
        TccWrapper(TccWrapper const&&) = delete;

        /// Deleted const move-assign-op to prevent move from const
        TccWrapper& operator = (TccWrapper const&&) = delete;

        /// Destroys tcc state
        ~TccWrapper() noexcept
        {
            M_Destroy();
        }

        /// Sets function for printing error messages
        void SetErrorCallback(void* userData, ErrorFunc_t function) const noexcept
        {
            tcc_set_error_func(m_state, userData, function);
        }

        /// Creates tcc compilation context, returns true on success
        bool CreateContext() noexcept
        {
            m_state = tcc_new();

            return m_state;
        }

        /// Destroys tcc compilation context
        void DestroyContext() noexcept
        {
            M_Destroy();
            M_Reset();
        }

        /// Sets options as from command line
        void SetOptions(char const* options) const noexcept
        {
            tcc_set_options(m_state, options);
        }

        /// Adds include path (as with -Ipath)
        void AddIncludePath(char const* path) const noexcept
        {
            tcc_add_include_path(m_state, path);
        }

        /// Adds system include path (as with -isystem path)
        void AddSystemIncludePath(char const* path) const noexcept
        {
            tcc_add_sysinclude_path(m_state, path);
        }

        /// Adds library path (as with -Lpath)
        void AddLibraryPath(char const* path) const noexcept
        {
            tcc_add_library_path(m_state, path);
        }

        /// Adds library (as with -lname)
        void AddLibrary(char const* name) const noexcept
        {
            tcc_add_library(m_state, name);
        }

        /// Adds file { C file, dll, object, library, ld script } for compilation, returns true on success
        bool AddFile(char const* path) const noexcept
        {
            return tcc_add_file(m_state, path) != -1;
        }

        /// Adds string containing C source for compilation, returns true on success
        bool AddSourceCode(char const* src) const noexcept
        {
            return tcc_compile_string(m_state, src) != -1;
        }

        /// Compiles code to auto-managed memory, returns true on success
        bool Compile() const noexcept
        {
            tcc_set_output_type(m_state, TCC_OUTPUT_MEMORY);

            return tcc_relocate(m_state, TCC_RELOCATE_AUTO) != -1;
        }

        /// Returns required size for the buffer to which code will be compiled or -1 on failure, call only once
        int32_t GetRequiredBufferSize() const noexcept
        {
            tcc_set_output_type(m_state, TCC_OUTPUT_MEMORY);

            return tcc_relocate(m_state, nullptr);
        }

        /// Compiles code to user defined buffer, returns true on success, call GetRequiredBufferSize() before this
        bool CompileToBuffer(void* buffer) const noexcept
        {
            return tcc_relocate(m_state, buffer) != -1;
        }

        /// Defines symbol with given name and (optional) value (as with #define name value)
        void Define(char const* name, char const* value = nullptr) const noexcept
        {
            tcc_define_symbol(m_state, name, value);
        }

        /// Undefines symbol with given name (as with #undef name)
        void Undefine(char const* name) const noexcept
        {
            tcc_undefine_symbol(m_state, name);
        }

        /// Adds symbol with given name
        void AddSymbol(char const* name, void const* symbol) const noexcept
        {
            tcc_add_symbol(m_state, name, symbol);
        }

        /// Returns void pointer to symbol with given name or nullptr if no such symbol exists
        void* GetSymbol(char const* name) const noexcept
        {
            return tcc_get_symbol(m_state, name);
        }

        /// Returns T pointer to symbol with given name or nullptr if no such symbol exists
        template <typename T>
        auto GetSymbolAs(char const* name) const noexcept
        {
            return detail::BitCast<T*>(GetSymbol(name));
        }

        /// Checks whether symbol with given name exists
        bool HasSymbol(char const* name) const noexcept
        {
            return GetSymbol(name) != nullptr;
        }

        /// Invokes function for each symbol registered in the context
        void ForEachSymbol(void* userData, ListSymbolsFunc_t function) const noexcept
        {
            tcc_list_symbols(m_state, userData, function);
        }

        /// Returns F pointer to function with given name or nullptr if no such symbol exists
        template <typename F>
        auto GetFunction(char const* name) const noexcept
        {
            if constexpr (std::is_function_v<F>)
            {
                return detail::BitCast<F*>(GetSymbol(name));
            }
            else
            {
                static_assert(detail::alwaysFalse<F>, "F is not a function!");
            }
        }

        /// Registers symbol from parameter as free function with given name, won't override if called multiple times
        template <typename F>
        void RegisterFunction(char const* name, F* functionPtr) const noexcept
        {
            if constexpr (std::is_function_v<F>)
            {
                tcc_add_symbol(m_state, name, detail::BitCast<void const*>(functionPtr));
            }
            else
            {
                static_assert(detail::alwaysFalse<F>, "F is not a function!");
            }
        }

        /// Registers symbol as free function with given name, won't override if called multiple times
        template <typename F, F vFunctionPtr>
        void RegisterFunction(char const* name) const noexcept
        {
            RegisterFunction(name, vFunctionPtr);
        }

        /// Registers symbol as free function with given name, won't override if called multiple times
        template <auto vFunctionPtr>
        void RegisterFunction(char const* name) const noexcept
        {
            RegisterFunction(name, vFunctionPtr);
        }

        /// Registers symbol as class method with given name, won't override if called multiple times
        template <typename M, M vMethodPtr>
        void RegisterMethod(char const* name) const noexcept
        {
            if constexpr (std::is_member_function_pointer_v<M>)
            {
                RegisterFunction(name, detail::AsFreeFunction<M, vMethodPtr>());
            }
            else
            {
                static_assert(detail::alwaysFalse<M>, "M is not a method!");
            }
        }

        /// Registers symbol as class method with given name, won't override if called multiple times
        template <auto vMethodPtr>
        void RegisterMethod(char const* name) const noexcept
        {
            RegisterMethod<decltype(vMethodPtr), vMethodPtr>(name);
        }

        #ifdef TW_USE_EXCEPTIONS

        /// Tries to call function with given args, throws if no such function symbol exists
        template <typename Ret, typename... Args>
        Ret Call(char const* name, Args&&... args) const
        {
            auto const symbol = GetFunction<Ret(Args...)>(name);

            if (symbol)
            {
                return (*symbol)(std::forward<Args>(args)...);
            }
            else
            {
                throw std::runtime_error(std::string{ "TccWrapper::Call() - unable to find symbol with given name: " } + name);
            }
        }

        #endif // TW_USE_EXCEPTIONS

        /// Tries to call function with given args, returns true on success
        template <typename Ret, typename... Args>
        bool CallSafely(char const* name, Ret& output, Args&&... args) const noexcept
        {
            auto const symbol = GetFunction<Ret(Args...)>(name);

            if (symbol)
            {
                output = (*symbol)(std::forward<Args>(args)...);

                return true;
            }
            else
            {
                return false;
            }
        }

        #ifdef TW_USE_OPTIONAL

        /// Tries to call function with given args, returns optional with call result
        template <typename Ret, typename... Args>
        std::optional<Ret> CallSafelyOpt(char const* name, Args&&... args) const noexcept
        {
            auto const symbol = GetFunction<Ret(Args...)>(name);

            if (symbol)
            {
                return std::make_optional<Ret>((*symbol)(std::forward<Args>(args)...));
            }
            else
            {
                return std::nullopt;
            }
        }

        #endif // TW_USE_OPTIONAL

        /// Outputs file depending on outputType, returns true on success
        bool OutputFile(char const* filename, OutputType outputType) const noexcept
        {
            tcc_set_output_type(m_state, +outputType);

            return tcc_output_file(m_state, filename) != -1;
        }

        /// Returns internal state on which tcc operates
        State_t GetState() const noexcept
        {
            return m_state;
        }

        /// Returns implicitly internal state on which tcc operates
        operator State_t() const noexcept
        {
            return m_state;
        }

    private:

        /// PRIVATE: Destroys internal state of the wrapper
        void M_Destroy() noexcept
        {
            if (m_state)
            {
                tcc_delete(m_state);
            }
        }

        /// PRIVATE: Resets internal state of the wrapper
        void M_Reset() noexcept
        {
            m_state = nullptr;
        }

        State_t m_state;
    };
}
