#include "List.hh"

#include <cassert>
#include <cstdio>
#include <cstring>

namespace ingot::codegen
{
#ifndef INGOT_RUNTIME    
    template <typename T, size_t blocksize>
    llvm::Function*
    List<T, blocksize>::m_llvmFunctionEmpty = nullptr;

    template <typename T, size_t blocksize>
    llvm::Function*
    List<T, blocksize>::m_llvmFunctionAppend = nullptr;

    template <typename T, size_t blocksize>
    llvm::Function*
    List<T, blocksize>::m_llvmFunctionFromZeroTermArray = nullptr;
#endif

    template <typename T, size_t blocksize>
    List<T, blocksize>
    List<T, blocksize>::empty() {
        List list;
        list.m_next = nullptr;
        list.m_count = 0;
        return list;
    }

    template <typename T, size_t blocksize>
    bool
    List<T, blocksize>::isEmpty() const {
        return m_count == 0;
    }

    template <typename T, size_t blocksize>
    List<T, blocksize>
    List<T, blocksize>::append(T t) const {
        if (m_count == blocksize) {
            List list;
            list.m_next = this;
            list.m_elements[blocksize - 1] = t;
            list.m_count = 1;
            return list;
        }
        List list = *this;
        list.m_elements[blocksize - 1 - list.m_count] = t;
        list.m_count += 1;
        return list;
    }

    template <typename T, size_t blocksize>
    T
    List<T, blocksize>::head() const {
        assert(m_count > 0);
        return m_elements[blocksize - m_count];
    }

    template <typename T, size_t blocksize>
    List<T, blocksize>
    List<T, blocksize>::tail() const {
        assert(m_count > 0);
        if (m_count == 1) {
            if (m_next) {
                return *m_next;
            } else {
                return empty();
            }
        }
        List copy = *this;
        copy.m_count -= 1;
        return copy;
    }

    template <typename T, size_t blocksize>
    List<T, blocksize>
    List<T, blocksize>::fromZeroTerminatedArray(const T* begin) {
        const T* end = nullptr;
        for (const T* p = begin; *p; ++p) {
            const T* next = p + 1;
            if (*next == T{}) {
                end = next;
            }
        }
        List current;
        current.m_next = nullptr;
        current.m_count = 0;
        const T* cursor = end - blocksize;
        while (cursor >= begin) {
            List list;
            if (current.m_count == blocksize) {
                list.m_next = new List(current);
            } else {
                list.m_next = nullptr;
            }
            current = list;
            memcpy(list.m_elements, cursor, blocksize);
            list.m_count = blocksize;
            cursor -= blocksize;
        }
        List list;
        if (current.m_count == blocksize) {
            list.m_next = new List(current);
        } else {
            list.m_next = nullptr;
        }
        list.m_count = cursor - begin + blocksize;
        memcpy(list.m_elements + blocksize - list.m_count, begin, list.m_count);
        return list;
    }

#ifndef INGOT_RUNTIME
    template <typename T, size_t blocksize>
    llvm::StructType*
    List<T, blocksize>::getLLVMType(llvm::IRBuilder<>& builder, const std::string& prefix) {
        std::string structName = "ingot_runtime_list_" + prefix + "_wrapper";
        constexpr size_t size = sizeof(List<T, blocksize>);
        llvm::ArrayType* arrayType = llvm::ArrayType::get(builder.getInt8Ty(), size);
        llvm::StructType* structType = llvm::StructType::get(arrayType);
        structType->setName(structName);
        return structType;
    }

    template <typename T, size_t blocksize>
    llvm::Function*
    List<T, blocksize>::getLLVMFunctionEmpty(llvm::Module& module, llvm::IRBuilder<>& builder, const std::string& prefix) {
        if (!m_llvmFunctionEmpty) {
            std::string symbol = "ingot_runtime_list_" + prefix + "_empty";
            llvm::StructType* structType = getLLVMType(builder, prefix);
            llvm::PointerType* pointerType = llvm::PointerType::get(structType, 0);
            llvm::FunctionType* prototype = llvm::FunctionType::get(builder.getVoidTy(), {pointerType}, false);
            m_llvmFunctionEmpty = llvm::Function::Create(prototype, llvm::Function::ExternalLinkage, symbol, &module);
        }
        return m_llvmFunctionEmpty;
    }

    template <typename T, size_t blocksize>
    llvm::Function*
    List<T, blocksize>::getLLVMFunctionAppend(llvm::Module& module, llvm::IRBuilder<>& builder, const std::string& prefix) {
        if (!m_llvmFunctionAppend) {
            std::string symbol = "ingot_runtime_list_" + prefix + "_append";
            llvm::StructType* structType = getLLVMType(builder, prefix);
            llvm::PointerType* pointerType = llvm::PointerType::get(structType, 0);
            llvm::FunctionType* prototype = llvm::FunctionType::get(builder.getVoidTy(), {builder.getInt8Ty() /*fix me for other types*/, pointerType, pointerType},false);
            m_llvmFunctionAppend = llvm::Function::Create(prototype, llvm::Function::ExternalLinkage, symbol, &module);
        }
        return m_llvmFunctionAppend;
    }

    template <typename T, size_t blocksize>
    llvm::Function*
    List<T, blocksize>::getLLVMFunctionFromZeroTermArray(llvm::Module& module, llvm::IRBuilder<>& builder, const std::string& prefix) {
        if (!m_llvmFunctionFromZeroTermArray) {
            std::string symbol = "ingot_runtime_list_" + prefix + "_from_zero_term_array";
            llvm::StructType* structType = getLLVMType(builder, prefix);
            llvm::PointerType* pointerType = llvm::PointerType::get(structType, 0);
            llvm::FunctionType* prototype = llvm::FunctionType::get(builder.getVoidTy(), {builder.getInt8PtrTy() /*fix me for other types*/, pointerType},false);
            m_llvmFunctionFromZeroTermArray = llvm::Function::Create(prototype, llvm::Function::ExternalLinkage, symbol, &module);
        }
        return m_llvmFunctionFromZeroTermArray;
    }
#endif
} // namespace ingot::codegen


#define INGOT_DEFINE_LIST_C_FUNCS(prefix, elemtype, blocksize) \
    void ingot_runtime_list_ ## prefix ## _empty(struct ingot_runtime_list_ ## prefix ## _wrapper* out) { \
        out->list = ingot::codegen::List<elemtype, blocksize>::empty(); \
    } \
    bool ingot_runtime_list_ ## prefix ## _is_empty(struct ingot_runtime_list_ ## prefix ## _wrapper* wrapped_list) { \
        return wrapped_list->list.isEmpty(); \
    } \
    void ingot_runtime_list_ ## prefix ## _append(char t, struct ingot_runtime_list_ ## prefix ## _wrapper* wrapped_list, struct ingot_runtime_list_ ## prefix ## _wrapper* out) { \
        out->list = wrapped_list->list.append(t); \
    } \
    void ingot_runtime_list_ ## prefix ## _from_zero_term_array(elemtype* begin, struct ingot_runtime_list_ ## prefix ## _wrapper* out) { \
        out->list = ingot::codegen::List<elemtype, blocksize>::fromZeroTerminatedArray(begin); \
    } \

#define INGOT_INSTANIATE_LIST(elemtype, blocksize) \
    namespace ingot::codegen { \
        template class List<elemtype, blocksize>; \
    }

#define INGOT_GENERATE_LIST(prefix, elemtype, blocksize) \
    INGOT_DEFINE_LIST_C_FUNCS(prefix, elemtype, blocksize) \
    INGOT_INSTANIATE_LIST(elemtype, blocksize)


INGOT_GENERATE_LIST(i8, char, 15)
INGOT_GENERATE_LIST(i64, int64_t, 15)

extern "C" {
    void ingot_runtime_print_string(struct ingot_runtime_list_i8_wrapper* wrapped_list);
}


void ingot_runtime_print_string(struct ingot_runtime_list_i8_wrapper* wrapped_list) {
    struct ingot::codegen::List<char, 15> copy = wrapped_list->list;
    while (!copy.isEmpty()) {
        putchar(copy.head());
        copy = copy.tail();
    }
}