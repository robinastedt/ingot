#pragma once

#include <boost/integer.hpp>
#ifndef INGOT_RUNTIME
#include <llvm/IR/IRBuilder.h>
#endif

namespace ingot::codegen
{
    template <typename T, size_t blocksize>
    class List {
        using CounterType = typename boost::int_max_value_t<blocksize>::least;
        const List* m_next;
        T m_elements[blocksize];
        CounterType m_count; // TODO: Some calculations below could be simplied if we tracked the index to the last element or the next to be inserted element
#ifndef INGOT_RUNTIME
        static llvm::Function* m_llvmFunctionEmpty;
        static llvm::Function* m_llvmFunctionAppend;
        static llvm::Function* m_llvmFunctionFromZeroTermArray;
#endif
    public:
        static List empty();
        bool isEmpty() const;
        List append(T t) const;
        T head() const;
        List tail() const;
        static List fromZeroTerminatedArray(const T*);
#ifndef INGOT_RUNTIME
        static llvm::StructType* getLLVMType(llvm::IRBuilder<>& builder, const std::string& prefix);
        static llvm::Function* getLLVMFunctionEmpty(llvm::Module& module, llvm::IRBuilder<>& builder, const std::string& prefix);
        static llvm::Function* getLLVMFunctionAppend(llvm::Module& module, llvm::IRBuilder<>& builder, const std::string& prefix);
        static llvm::Function* getLLVMFunctionFromZeroTermArray(llvm::Module& module, llvm::IRBuilder<>& builder, const std::string& prefix);
#endif
    };

} // namespace ingot::codegen

#define INGOT_DECLARE_LIST_C_FUNCS(prefix, elemtype, blocksize) \
    extern "C" { \
        struct ingot_runtime_list_ ## prefix ## _wrapper { \
            ingot::codegen::List<elemtype, blocksize> list; \
        }; \
        void ingot_runtime_list_ ## prefix ## _empty(struct ingot_runtime_list_ ## prefix ## _wrapper* out); \
        bool ingot_runtime_list_ ## prefix ## _is_empty(struct ingot_runtime_list_ ## prefix ## _wrapper* wrapped_list); \
        void ingot_runtime_list_ ## prefix ## _append(elemtype t, struct ingot_runtime_list_ ## prefix ## _wrapper* wrapped_list, struct ingot_runtime_list_ ## prefix ## _wrapper* out); \
        void ingot_runtime_list_ ## prefix ## _from_zero_term_array(elemtype* begin, struct ingot_runtime_list_ ## prefix ## _wrapper* out); \
    }



INGOT_DECLARE_LIST_C_FUNCS(i8, char, 15)
INGOT_DECLARE_LIST_C_FUNCS(i64, int64_t, 15)
