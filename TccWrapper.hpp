/*

    Convenient header-only C++ wrapper for use with embedded Tiny C Compiler

    Created by Patrick Stritch

    Macros:
    * TW_USE_OPTIONAL
    * TW_PROHIBIT_EXCEPTIONS

*/

#pragma once

// C++
#if defined(TW_USE_OPTIONAL)
#include <optional>
#endif // TW_USE_OPTIONAL
#if !defined(TW_PROHIBIT_EXCEPTIONS)
#include <stdexcept>
#endif // TW_PROHIBIT_EXCEPTIONS
#include <type_traits>

#include <iostream>

// tcc
#include "libtcc.h"

namespace tw
{

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

        /// Initialize tcc state to nullptr
        TccWrapper() = default;

        /// Deleted copy-ctor
        TccWrapper(const TccWrapper&) = delete;

        /// Deleted copy-assign-op
        TccWrapper& operator = (const TccWrapper&) = delete;

        /// Move state from the rhs
        TccWrapper(TccWrapper&& rhs)
            : m_state { rhs.m_state }
        {
            rhs.m_state = nullptr;
        }

        /// Move state from the rhs and delete current if possible
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

        /// Destroy tcc state if possible
        ~TccWrapper()
        {
            if (m_state)
            {
                tcc_delete(m_state);
            }
        }

        /// Set callback for printing error messages (void* for userData, const char* for message)
        void SetErrorCallback(void* userData, void (*function)(void*, const char*))
        {
            tcc_set_error_func(m_state, userData, function);
        }

        /// Create tcc compilation context, return true on success
        bool CreateContext()
        {
            m_state = tcc_new();

            return m_state;
        }

        /// Set compilation output to the one of { Dll, Executable, Memory, Object }
        void SetOutputType(OutputType outputType)
        {
            tcc_set_output_type(m_state, static_cast<int>(outputType));
        }

        /// Set options as from command line
        void SetOptions(const char* options)
        {
            tcc_set_options(m_state, options);
        }

        /// Add include path (as with -Ipath)
        void AddIncludePath(const char* path) const
        {
            tcc_add_include_path(m_state, path);
        }

        /// Add system include path (as with -isystem path)
        void AddSystemIncludePath(const char* path) const
        {
            tcc_add_sysinclude_path(m_state, path);
        }

        /// Add library path (as with -Lpath)
        void AddLibraryPath(const char* path) const
        {
            tcc_add_library_path(m_state, path);
        }

        /// Add library (as with -lname)
        void AddLibrary(const char* name) const
        {
            tcc_add_library(m_state, name);
        }

        /// Add file { C file, dll, object, library, ld script } for compilation, return true on success
        bool AddFile(const char* path) const
        {
            return tcc_add_file(m_state, path) != -1;
        }

        /// Add string containing C source for compilation, return true on success
        bool AddSourceCode(const char* src)
        {
            return tcc_compile_string(m_state, src) != -1;
        }

        /// Compile code to auto-managed memory
        bool CompileToMemory() const
        {
            return tcc_relocate(m_state, TCC_RELOCATE_AUTO) != -1;
        }

        /// Define symbol with given name and (optional) value (as with #define name value)
        void Define(const char* name, const char* value = nullptr) const
        {
            tcc_define_symbol(m_state, name, value);
        }

        /// Undefine symbol with given name (as with #undef name)
        void Undefine(const char* name) const
        {
            tcc_undefine_symbol(m_state, name);
        }

        /// Add symbol with given name
        void AddSymbol(const char* name, const void* symbol) const
        {
            tcc_add_symbol(m_state, name, symbol);
        }

        /// Return void pointer to symbol with given name or nullptr if no such symbol exists
        void* GetSymbol(const char* name) const
        {
            return tcc_get_symbol(m_state, name);
        }

        /// Return T pointer to symbol with given name or nullptr if no such symbol exists
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

        /// Check whether symbol with given name exists
        bool HasSymbol(const char* name) const
        {
            return GetSymbol(name) != nullptr;
        }

        /// Register symbol as free function with given name, won't override if called multiple times
        template <typename Ret, typename... Args>
        void RegisterFunction(const char* name, Ret (*function)(Args...)) const
        {
            tcc_add_symbol(m_state, name, reinterpret_cast<const void*>(function));
        }

        #if !defined(TW_PROHIBIT_EXCEPTIONS)

        /// Try to call function with given args, throw if no such function symbol exists
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

        #endif // TW_PROHIBIT_EXCEPTIONS

        /// Try to call function with given args, return true on success
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

        #if defined(TW_USE_OPTIONAL)

        /// Try to call function with given args, return optional with call result
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

        TCCState* m_state = nullptr; ///< Internal state on which tcc operates
    };

}
