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
#include <type_traits>
#include <utility>

// tcc
#include "libtcc.h"

namespace tw
{
    namespace detail
    {
        template <typename Class, typename Ret, typename... Args>
        struct MethodTraitsBase
        {
            using Class_t  = Class;
            using Return_t = Ret;
            using Args_t   = std::tuple<Args...>;
            template <size_t N>
            using NthArg_t = std::tuple_element_t<N, Args_t>;

            inline static constexpr auto arity = sizeof...(Args);
        };

        template <typename>
        class MethodTraits;

        template <typename Class, typename Ret, typename... Args>
        struct MethodTraits<Ret(Class::*)(Args...)> : MethodTraitsBase<Class, Ret, Args...> {};

        template <typename Class, typename Ret, typename... Args>
        struct MethodTraits<Ret(Class::*)(Args...) const> : MethodTraitsBase<const Class, Ret, Args...> {};
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

        /// Initializes tcc state to nullptr
        TccWrapper() = default;

        /// Deleted copy-ctor
        TccWrapper(const TccWrapper&) = delete;

        /// Deleted copy-assign-op
        TccWrapper& operator = (const TccWrapper&) = delete;

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

        /// Sets callback for printing error messages (void* for userData, const char* for message)
        void SetErrorCallback(void* userData, void (*function)(void*, const char*))
        {
            tcc_set_error_func(m_state, userData, function);
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
            tcc_set_output_type(m_state, static_cast<int>(outputType));
        }

        /// Sets options as from command line
        void SetOptions(const char* options)
        {
            tcc_set_options(m_state, options);
        }

        /// Adds include path (as with -Ipath)
        void AddIncludePath(const char* path) const
        {
            tcc_add_include_path(m_state, path);
        }

        /// Adds system include path (as with -isystem path)
        void AddSystemIncludePath(const char* path) const
        {
            tcc_add_sysinclude_path(m_state, path);
        }

        /// Adds library path (as with -Lpath)
        void AddLibraryPath(const char* path) const
        {
            tcc_add_library_path(m_state, path);
        }

        /// Adds library (as with -lname)
        void AddLibrary(const char* name) const
        {
            tcc_add_library(m_state, name);
        }

        /// Adds file { C file, dll, object, library, ld script } for compilation, returns true on success
        bool AddFile(const char* path) const
        {
            return tcc_add_file(m_state, path) != -1;
        }

        /// Adds string containing C source for compilation, returns true on success
        bool AddSourceCode(const char* src)
        {
            return tcc_compile_string(m_state, src) != -1;
        }

        /// Compiles code to auto-managed memory
        bool CompileToMemory() const
        {
            return tcc_relocate(m_state, TCC_RELOCATE_AUTO) != -1;
        }

        /// Defines symbol with given name and (optional) value (as with #define name value)
        void Define(const char* name, const char* value = nullptr) const
        {
            tcc_define_symbol(m_state, name, value);
        }

        /// Undefines symbol with given name (as with #undef name)
        void Undefine(const char* name) const
        {
            tcc_undefine_symbol(m_state, name);
        }

        /// Adds symbol with given name
        void AddSymbol(const char* name, const void* symbol) const
        {
            tcc_add_symbol(m_state, name, symbol);
        }

        /// Returns void pointer to symbol with given name or nullptr if no such symbol exists
        void* GetSymbol(const char* name) const
        {
            return tcc_get_symbol(m_state, name);
        }

        /// Returns T pointer to symbol with given name or nullptr if no such symbol exists
        template <typename T>
        auto GetSymbolAs(const char* name) const
        {
            const auto symbol = GetSymbol(name);

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
        bool HasSymbol(const char* name) const
        {
            return GetSymbol(name) != nullptr;
        }

        /// Registers symbol from parameter as free function with given name, won't override if called multiple times
        template <typename Ret, typename... Args>
        void RegisterFunction(const char* name, Ret (*function)(Args...)) const
        {
            tcc_add_symbol(m_state, name, reinterpret_cast<const void*>(function));
        }

        /// Registers symbol as free function with given name, won't override if called multiple times
        template <auto FunctionPtr>
        void RegisterFunction(const char* name) const
        {
            tcc_add_symbol(m_state, name, reinterpret_cast<const void*>(FunctionPtr));
        }

        /// Registers symbol as class method with given name, won't override if called multiple times
        template <auto MethodPtr>
        void RegisterMethod(const char* name) const
        {
            using Traits_t = detail::MethodTraits<decltype(MethodPtr)>;

            M_RegisterMethod<MethodPtr>(name, std::make_index_sequence<Traits_t::arity>{});
        }

        #ifdef TW_USE_EXCEPTIONS

        /// Tries to call function with given args, throws if no such function symbol exists
        template <typename Ret, typename... Args>
        Ret Call(const char* name, Args&&... args) const
        {
            const auto symbol = GetSymbolAs<Ret(*)(Args...)>(name);

            if (symbol)
            {
                return (*symbol)(std::forward<Args>(args)...);
            }
            else
            {
                throw std::runtime_error("TccWrapper::Call() - unable to find symbol with given name");
            }
        }

        #endif // TW_USE_EXCEPTIONS

        /// Tries to call function with given args, returns true on success
        template <typename Ret, typename... Args>
        bool CallSafely(const char* name, Ret& output, Args&&... args) const
        {
            const auto symbol = GetSymbolAs<Ret(*)(Args...)>(name);

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
        std::optional<Ret> CallSafelyOpt(const char* name, Args&&... args) const
        {
            const auto symbol = GetSymbolAs<Ret(*)(Args...)>(name);

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

    private:

        /// Internal helper for RegisterMethod method
        template <auto MethodPtr, size_t... Is>
        void M_RegisterMethod(const char* name, std::index_sequence<Is...>) const
        {
            using Traits_t = detail::MethodTraits<decltype(MethodPtr)>;

            auto function = +[](typename Traits_t::Class_t* ptr, typename Traits_t::template NthArg_t<Is>... args) -> typename Traits_t::Return_t {
                return (ptr->*MethodPtr)(std::forward<decltype(args)>(args)...);
            };

            RegisterFunction(name, function);
        }

        TCCState* m_state = nullptr; ///< Internal state on which tcc operates
    };
}
