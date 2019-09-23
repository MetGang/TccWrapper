/*

    Convenient header-only C++ wrapper for use with embedded Tiny C Compiler

    Created by Patrick Stritch

    Macros:
    * TW_USE_OPTIONAL
    * TW_USE_EXCEPTIONS

*/

#pragma once

// C++
#ifdef TW_USE_OPTIONAL
#include <optional>
#endif // TW_USE_OPTIONAL
#ifdef TW_USE_EXCEPTIONS
#include <stdexcept>
#endif // TW_USE_EXCEPTIONS
#include <tuple>

extern "C"
{
    struct TCCState;

    typedef struct TCCState TCCState;

    #define TCC_OUTPUT_MEMORY     1
    #define TCC_OUTPUT_EXE        2
    #define TCC_OUTPUT_DLL        3
    #define TCC_OUTPUT_OBJ        4
    #define TCC_OUTPUT_PREPROCESS 5

    #define TCC_RELOCATE_AUTO     (void*)1

    TCCState* tcc_new(void);

    void tcc_delete(TCCState* state);

    void tcc_set_lib_path(TCCState* state, char const* path);

    void tcc_set_error_func(TCCState* state, void* userData, void (*function)(void* userData, char const* msg));

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
}

namespace tw
{
    namespace detail
    {
        template <typename PureClass, typename Class, typename Ret, bool vIsNoexcept, typename... Args>
        struct MethodTraitsBase
        {
            using PureClass_t = PureClass;
            using Class_t     = Class;
            using Return_t    = Ret;
            using Args_t      = std::tuple<Args...>;
            template <size_t N>
            using NthArg_t    = std::tuple_element_t<N, Args_t>;

            inline static constexpr auto arity      = sizeof...(Args);
            inline static constexpr auto isNoexcept = vIsNoexcept;
        };

        template <typename>
        struct MethodTraits;

        template <typename Class, typename Ret, typename... Args>
        struct MethodTraits<Ret(Class::*)(Args...)> : MethodTraitsBase<Class, Class, Ret, false, Args...> {};

        template <typename Class, typename Ret, typename... Args>
        struct MethodTraits<Ret(Class::*)(Args...) const> : MethodTraitsBase<Class, Class const, Ret, false, Args...> {};

        template <typename Class, typename Ret, typename... Args>
        struct MethodTraits<Ret(Class::*)(Args...) volatile> : MethodTraitsBase<Class, Class volatile, Ret, false, Args...> {};

        template <typename Class, typename Ret, typename... Args>
        struct MethodTraits<Ret(Class::*)(Args...) const volatile> : MethodTraitsBase<Class, Class const volatile, Ret, false, Args...> {};

        template <typename Class, typename Ret, typename... Args>
        struct MethodTraits<Ret(Class::*)(Args..., ...)> : MethodTraitsBase<Class, Class, Ret, false, Args...> {};

        template <typename Class, typename Ret, typename... Args>
        struct MethodTraits<Ret(Class::*)(Args..., ...) const> : MethodTraitsBase<Class, Class const, Ret, false, Args...> {};

        template <typename Class, typename Ret, typename... Args>
        struct MethodTraits<Ret(Class::*)(Args..., ...) volatile> : MethodTraitsBase<Class, Class volatile, Ret, false, Args...> {};

        template <typename Class, typename Ret, typename... Args>
        struct MethodTraits<Ret(Class::*)(Args..., ...) const volatile> : MethodTraitsBase<Class, Class const volatile, Ret, false, Args...> {};

        template <typename Class, typename Ret, typename... Args>
        struct MethodTraits<Ret(Class::*)(Args...) &> : MethodTraitsBase<Class, Class&, Ret, false, Args...> {};

        template <typename Class, typename Ret, typename... Args>
        struct MethodTraits<Ret(Class::*)(Args...) const&> : MethodTraitsBase<Class, Class const&, Ret, false, Args...> {};

        template <typename Class, typename Ret, typename... Args>
        struct MethodTraits<Ret(Class::*)(Args...) volatile&> : MethodTraitsBase<Class, Class volatile&, Ret, false, Args...> {};

        template <typename Class, typename Ret, typename... Args>
        struct MethodTraits<Ret(Class::*)(Args...) const volatile&> : MethodTraitsBase<Class, Class const volatile&, Ret, false, Args...> {};

        template <typename Class, typename Ret, typename... Args>
        struct MethodTraits<Ret(Class::*)(Args..., ...) &> : MethodTraitsBase<Class, Class&, Ret, false, Args...> {};

        template <typename Class, typename Ret, typename... Args>
        struct MethodTraits<Ret(Class::*)(Args..., ...) const&> : MethodTraitsBase<Class, Class const&, Ret, false, Args...> {};

        template <typename Class, typename Ret, typename... Args>
        struct MethodTraits<Ret(Class::*)(Args..., ...) volatile&> : MethodTraitsBase<Class, Class volatile&, Ret, false, Args...> {};

        template <typename Class, typename Ret, typename... Args>
        struct MethodTraits<Ret(Class::*)(Args..., ...) const volatile&> : MethodTraitsBase<Class, Class const volatile&, Ret, false, Args...> {};

        template <typename Class, typename Ret, typename... Args>
        struct MethodTraits<Ret(Class::*)(Args...) &&> : MethodTraitsBase<Class, Class&&, Ret, false, Args...> {};

        template <typename Class, typename Ret, typename... Args>
        struct MethodTraits<Ret(Class::*)(Args...) const&&> : MethodTraitsBase<Class, Class const&&, Ret, false, Args...> {};

        template <typename Class, typename Ret, typename... Args>
        struct MethodTraits<Ret(Class::*)(Args...) volatile&&> : MethodTraitsBase<Class, Class volatile&&, Ret, false, Args...> {};

        template <typename Class, typename Ret, typename... Args>
        struct MethodTraits<Ret(Class::*)(Args...) const volatile&&> : MethodTraitsBase<Class, Class const volatile&&, Ret, false, Args...> {};

        template <typename Class, typename Ret, typename... Args>
        struct MethodTraits<Ret(Class::*)(Args..., ...) &&> : MethodTraitsBase<Class, Class&&, Ret, false, Args...> {};

        template <typename Class, typename Ret, typename... Args>
        struct MethodTraits<Ret(Class::*)(Args..., ...) const&&> : MethodTraitsBase<Class, Class const&&, Ret, false, Args...> {};

        template <typename Class, typename Ret, typename... Args>
        struct MethodTraits<Ret(Class::*)(Args..., ...) volatile&&> : MethodTraitsBase<Class, Class volatile&&, Ret, false, Args...> {};

        template <typename Class, typename Ret, typename... Args>
        struct MethodTraits<Ret(Class::*)(Args..., ...) const volatile&&> : MethodTraitsBase<Class, Class const volatile&&, Ret, false, Args...> {};

        template <typename Class, typename Ret, typename... Args>
        struct MethodTraits<Ret(Class::*)(Args...) noexcept> : MethodTraitsBase<Class, Class, Ret, true, Args...> {};

        template <typename Class, typename Ret, typename... Args>
        struct MethodTraits<Ret(Class::*)(Args...) const noexcept> : MethodTraitsBase<Class, Class const, Ret, true, Args...> {};

        template <typename Class, typename Ret, typename... Args>
        struct MethodTraits<Ret(Class::*)(Args...) volatile noexcept> : MethodTraitsBase<Class, Class volatile, Ret, true, Args...> {};

        template <typename Class, typename Ret, typename... Args>
        struct MethodTraits<Ret(Class::*)(Args...) const volatile noexcept> : MethodTraitsBase<Class, Class const volatile, Ret, true, Args...> {};

        template <typename Class, typename Ret, typename... Args>
        struct MethodTraits<Ret(Class::*)(Args..., ...) noexcept> : MethodTraitsBase<Class, Class, Ret, true, Args...> {};

        template <typename Class, typename Ret, typename... Args>
        struct MethodTraits<Ret(Class::*)(Args..., ...) const noexcept> : MethodTraitsBase<Class, Class const, Ret, true, Args...> {};

        template <typename Class, typename Ret, typename... Args>
        struct MethodTraits<Ret(Class::*)(Args..., ...) volatile noexcept> : MethodTraitsBase<Class, Class volatile, Ret, true, Args...> {};

        template <typename Class, typename Ret, typename... Args>
        struct MethodTraits<Ret(Class::*)(Args..., ...) const volatile noexcept> : MethodTraitsBase<Class, Class const volatile, Ret, true, Args...> {};

        template <typename Class, typename Ret, typename... Args>
        struct MethodTraits<Ret(Class::*)(Args...) & noexcept> : MethodTraitsBase<Class, Class&, Ret, true, Args...> {};

        template <typename Class, typename Ret, typename... Args>
        struct MethodTraits<Ret(Class::*)(Args...) const& noexcept> : MethodTraitsBase<Class, Class const&, Ret, true, Args...> {};

        template <typename Class, typename Ret, typename... Args>
        struct MethodTraits<Ret(Class::*)(Args...) volatile& noexcept> : MethodTraitsBase<Class, Class volatile&, Ret, true, Args...> {};

        template <typename Class, typename Ret, typename... Args>
        struct MethodTraits<Ret(Class::*)(Args...) const volatile& noexcept> : MethodTraitsBase<Class, Class const volatile&, Ret, true, Args...> {};

        template <typename Class, typename Ret, typename... Args>
        struct MethodTraits<Ret(Class::*)(Args..., ...) & noexcept> : MethodTraitsBase<Class, Class&, Ret, true, Args...> {};

        template <typename Class, typename Ret, typename... Args>
        struct MethodTraits<Ret(Class::*)(Args..., ...) const& noexcept> : MethodTraitsBase<Class, Class const&, Ret, true, Args...> {};

        template <typename Class, typename Ret, typename... Args>
        struct MethodTraits<Ret(Class::*)(Args..., ...) volatile& noexcept> : MethodTraitsBase<Class, Class volatile&, Ret, true, Args...> {};

        template <typename Class, typename Ret, typename... Args>
        struct MethodTraits<Ret(Class::*)(Args..., ...) const volatile& noexcept> : MethodTraitsBase<Class, Class const volatile&, Ret, true, Args...> {};

        template <typename Class, typename Ret, typename... Args>
        struct MethodTraits<Ret(Class::*)(Args...) && noexcept> : MethodTraitsBase<Class, Class&&, Ret, true, Args...> {};

        template <typename Class, typename Ret, typename... Args>
        struct MethodTraits<Ret(Class::*)(Args...) const&& noexcept> : MethodTraitsBase<Class, Class const&&, Ret, true, Args...> {};

        template <typename Class, typename Ret, typename... Args>
        struct MethodTraits<Ret(Class::*)(Args...) volatile&& noexcept> : MethodTraitsBase<Class, Class volatile&&, Ret, true, Args...> {};

        template <typename Class, typename Ret, typename... Args>
        struct MethodTraits<Ret(Class::*)(Args...) const volatile&& noexcept> : MethodTraitsBase<Class, Class const volatile&&, Ret, true, Args...> {};

        template <typename Class, typename Ret, typename... Args>
        struct MethodTraits<Ret(Class::*)(Args..., ...) && noexcept> : MethodTraitsBase<Class, Class&&, Ret, true, Args...> {};

        template <typename Class, typename Ret, typename... Args>
        struct MethodTraits<Ret(Class::*)(Args..., ...) const&& noexcept> : MethodTraitsBase<Class, Class const&&, Ret, true, Args...> {};

        template <typename Class, typename Ret, typename... Args>
        struct MethodTraits<Ret(Class::*)(Args..., ...) volatile&& noexcept> : MethodTraitsBase<Class, Class volatile&&, Ret, true, Args...> {};

        template <typename Class, typename Ret, typename... Args>
        struct MethodTraits<Ret(Class::*)(Args..., ...) const volatile&& noexcept> : MethodTraitsBase<Class, Class const volatile&&, Ret, true, Args...> {};

        template <typename M, M vMethodPtr, size_t... Is>
        constexpr auto AsFreeFunction(std::index_sequence<Is...>)
        {
            using Traits_t = detail::MethodTraits<M>;

            using PureClass_t = typename Traits_t::PureClass_t;
            using Class_t     = typename Traits_t::Class_t;
            using Return_t    = typename Traits_t::Return_t;

            return +[](PureClass_t* ptr, typename Traits_t::template NthArg_t<Is>... args) noexcept(Traits_t::isNoexcept) -> Return_t {
                return (static_cast<Class_t&&>(*ptr).*vMethodPtr)(std::forward<decltype(args)>(args)...);
            };
        }

        template <typename...>
        inline constexpr bool dependentFalse = false;
    }

    ///
    template <typename M, M vMethodPtr>
    constexpr auto AsFreeFunction()
    {
        using Traits_t = detail::MethodTraits<M>;

        return detail::AsFreeFunction<M, vMethodPtr>(std::make_index_sequence<Traits_t::arity>{});
    }

    ///
    template <auto vMethodPtr>
    constexpr auto AsFreeFunction()
    {
        return AsFreeFunction<decltype(vMethodPtr), vMethodPtr>();
    }

    /// Enumeration that specifies how code will be compiled
    enum class OutputType : int32_t
    {
        Dll = TCC_OUTPUT_DLL,
        Executable = TCC_OUTPUT_EXE,
        Memory = TCC_OUTPUT_MEMORY,
        Object = TCC_OUTPUT_OBJ
    };

    /// Wrapper around TCCState* with set of useful methods
    class TccWrapper
    {
    public:

        template <typename T = void>
        using ErrorCallback_t = void (*)(T*, char const*);
        using State_t         = TCCState*;

        /// Initializes tcc state to nullptr
        TccWrapper() = default;

        /// Deleted copy-ctor
        TccWrapper(TccWrapper const&) = delete;

        /// Deleted copy-assign-op
        TccWrapper& operator = (TccWrapper const&) = delete;

        /// Moves state from the rhs
        TccWrapper(TccWrapper&& rhs)
            : m_state { rhs.m_state }
        {
            rhs.m_state = nullptr;
        }

        /// Moves state from the rhs and deletes current if possible
        TccWrapper& operator = (TccWrapper&& rhs)
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

        /// Destroys tcc state if possible
        ~TccWrapper()
        {
            if (m_state)
            {
                tcc_delete(m_state);
            }
        }

        /// Sets callback for printing error messages (void* for userData, char const* for message)
        void SetErrorCallback(void* userData, ErrorCallback_t<> callback)
        {
            tcc_set_error_func(m_state, userData, callback);
        }

        /// Sets callback for printing error messages (void* for userData, char const* for message)
        template <typename T>
        void SetExtErrorCallback(T* userData, ErrorCallback_t<T> callback)
        {
            tcc_set_error_func(m_state, userData, reinterpret_cast<ErrorCallback_t<>>(callback));
        }

        /// Creates tcc compilation context, returns true on success
        bool CreateContext()
        {
            m_state = tcc_new();

            return m_state;
        }

        /// Sets compilation output to the one of { Dll, Executable, Memory, Object }
        void SetOutputType(OutputType outputType)
        {
            tcc_set_output_type(m_state, static_cast<std::underlying_type_t<OutputType>>(outputType));
        }

        /// Sets options as from command line
        void SetOptions(char const* options)
        {
            tcc_set_options(m_state, options);
        }

        /// Adds include path (as with -Ipath)
        void AddIncludePath(char const* path) const
        {
            tcc_add_include_path(m_state, path);
        }

        /// Adds system include path (as with -isystem path)
        void AddSystemIncludePath(char const* path) const
        {
            tcc_add_sysinclude_path(m_state, path);
        }

        /// Adds library path (as with -Lpath)
        void AddLibraryPath(char const* path) const
        {
            tcc_add_library_path(m_state, path);
        }

        /// Adds library (as with -lname)
        void AddLibrary(char const* name) const
        {
            tcc_add_library(m_state, name);
        }

        /// Adds file { C file, dll, object, library, ld script } for compilation, returns true on success
        bool AddFile(char const* path) const
        {
            return tcc_add_file(m_state, path) != -1;
        }

        /// Adds string containing C source for compilation, returns true on success
        bool AddSourceCode(char const* src)
        {
            return tcc_compile_string(m_state, src) != -1;
        }

        /// Compiles code to auto-managed memory, returns true on success
        bool Compile() const
        {
            return tcc_relocate(m_state, TCC_RELOCATE_AUTO) != -1;
        }

        /// Defines symbol with given name and (optional) value (as with #define name value)
        void Define(char const* name, char const* value = nullptr) const
        {
            tcc_define_symbol(m_state, name, value);
        }

        /// Undefines symbol with given name (as with #undef name)
        void Undefine(char const* name) const
        {
            tcc_undefine_symbol(m_state, name);
        }

        /// Adds symbol with given name
        void AddSymbol(char const* name, void const* symbol) const
        {
            tcc_add_symbol(m_state, name, symbol);
        }

        /// Returns void pointer to symbol with given name or nullptr if no such symbol exists
        void* GetSymbol(char const* name) const
        {
            return tcc_get_symbol(m_state, name);
        }

        /// Returns T pointer to symbol with given name or nullptr if no such symbol exists
        template <typename T>
        auto GetSymbolAs(char const* name) const
        {
            auto const symbol = GetSymbol(name);

            if constexpr (std::is_function_v<std::remove_pointer_t<T>>)
            {
                return reinterpret_cast<T>(symbol);
            }
            else
            {
                return reinterpret_cast<T*>(symbol);
            }
        }

        /// Checks whether symbol with given name exists
        bool HasSymbol(char const* name) const
        {
            return GetSymbol(name) != nullptr;
        }

        /// Registers symbol from parameter as free function with given name, won't override if called multiple times
        template <typename Ret, typename... Args>
        void RegisterFunction(char const* name, Ret (*function)(Args...)) const
        {
            tcc_add_symbol(m_state, name, reinterpret_cast<void const*>(function));
        }

        /// Registers symbol as free function with given name, won't override if called multiple times
        template <typename F, F vFunctionPtr>
        void RegisterFunction(char const* name) const
        {
            if constexpr (std::is_function_v<std::remove_pointer_t<F>>)
            {
                RegisterFunction(name, vFunctionPtr);
            }
            else
            {
                static_assert(detail::dependentFalse<F>, "vFunctionPtr is not a function!");
            }
        }

        /// Registers symbol as free function with given name, won't override if called multiple times
        template <auto vFunctionPtr>
        void RegisterFunction(char const* name) const
        {
            RegisterFunction<decltype(vFunctionPtr), vFunctionPtr>(name);
        }

        /// Registers symbol as class method with given name, won't override if called multiple times
        template <typename M, M vMethodPtr>
        void RegisterMethod(char const* name) const
        {
            if constexpr (std::is_member_function_pointer_v<M>)
            {
                RegisterFunction(name, AsFreeFunction<M, vMethodPtr>());
            }
            else
            {
                static_assert(detail::dependentFalse<M>, "vMethodPtr is not a method!");
            }
        }

        /// Registers symbol as class method with given name, won't override if called multiple times
        template <auto vMethodPtr>
        void RegisterMethod(char const* name) const
        {
            RegisterMethod<decltype(vMethodPtr), vMethodPtr>(name);
        }

        #ifdef TW_USE_EXCEPTIONS

        /// Tries to call function with given args, throws if no such function symbol exists
        template <typename Ret, typename... Args>
        Ret Call(char const* name, Args&&... args) const
        {
            auto const symbol = GetSymbolAs<Ret(*)(Args...)>(name);

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
        bool CallSafely(char const* name, Ret& output, Args&&... args) const
        {
            auto const symbol = GetSymbolAs<Ret(*)(Args...)>(name);

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
        std::optional<Ret> CallSafelyOpt(char const* name, Args&&... args) const
        {
            auto const symbol = GetSymbolAs<Ret(*)(Args...)>(name);

            if (symbol)
            {
                return { (*symbol)(std::forward<Args>(args)...) };
            }
            else
            {
                return {};
            }
        }

        #endif // TW_USE_OPTIONAL

        /// Returns internal state on which tcc operates
        State_t GetState() const
        {
            return m_state;
        }

    private:

        State_t m_state = nullptr; ///< Internal state on which tcc operates
    };
}