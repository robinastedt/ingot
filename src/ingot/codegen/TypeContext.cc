#include "TypeContext.hh"

#include <ingot/Error.hh>

namespace ingot::codegen
{
    namespace {
        constexpr size_t
        getMinCounterSize(size_t arrayLen) {
            size_t size = 1;
            while ((1 << size) <= arrayLen) {
                ++size;
            }
            return size + 1; // sign bit;
        }
        // 15 elements aligns nicely together with counter member filling out the 16th member
        constexpr size_t LIST_ARRAY_LEN = 15;
        constexpr size_t LIST_COUNTER_SIZE = getMinCounterSize(LIST_ARRAY_LEN);
        constexpr size_t getMinimumListSize(size_t elementSize) {
            return sizeof(void*) + ((LIST_COUNTER_SIZE + 7) / 8) + (elementSize * LIST_ARRAY_LEN);
        }
        constexpr size_t LIST_I8_SIZE =getMinimumListSize(sizeof(char));
        constexpr size_t LIST_I64_SIZE =getMinimumListSize(sizeof(int64_t));
        // We need to represent 0 - LIST_ARRAY_LEN inclusive, and account for unused sign bit
        static_assert((1 << (LIST_COUNTER_SIZE - 1)) > LIST_ARRAY_LEN);
    }


    TypeContext::TypeContext(llvm::IRBuilder<>& builder, llvm::Module& module)
    : m_builder(builder)
    , m_module(module)
    , m_typeMap()
    , m_typeMapListArrayMember() {}

    llvm::ArrayType*
    TypeContext::getLLVMTypeForListArrayMember(const ast::Type& elementType) {
        llvm::ArrayType*& arrayType = m_typeMapListArrayMember[elementType];
        if (arrayType == nullptr) {
            llvm::Type* elementLLVMType = getLLVMType(elementType);
            arrayType = llvm::ArrayType::get(elementLLVMType, LIST_ARRAY_LEN);
        }
        return arrayType;
    }

    llvm::IntegerType*
    TypeContext::getLLVMTypeForListCounterMember() {
        return llvm::IntegerType::get(m_builder.getContext(), LIST_COUNTER_SIZE);
    }

    llvm::Type*
    TypeContext::getLLVMType(const ast::Type& type) {
        llvm::Type*& llvmtype = m_typeMap[type];
        if (llvmtype == nullptr) {
            switch (type.getVariant()) {
                case ast::Type::Variant::i8: {
                    llvmtype = llvm::IntegerType::get(m_builder.getContext(), 8);
                } break;
                case ast::Type::Variant::i64: {
                    llvmtype = llvm::IntegerType::get(m_builder.getContext(), 64);
                } break;
                case ast::Type::Variant::List: {
                    ast::Type subtype = type.getSubtype();
                    llvm::Type* llvmSubtype = getLLVMType(subtype);
                    llvm::StructType* structType = llvm::StructType::create(m_builder.getContext(), "ingot_" + type.getNameEncoded());
                    llvm::PointerType* pointerType = llvm::PointerType::get(structType, 0);
                    llvm::IntegerType* counterType = getLLVMTypeForListCounterMember();
                    llvm::ArrayType* arrayType = getLLVMTypeForListArrayMember(subtype);
                    structType->setBody({pointerType, arrayType, counterType});
                    llvmtype = structType;
                } break;
                default: internal_error("Unhandled type variant: " + type.getName());
            }
        }
        return llvmtype;
    }

    llvm::Module&
    TypeContext::getModule() const {
        return m_module;
    }

    llvm::IRBuilder<>&
    TypeContext::getBuilder() const {
        return m_builder;
    }

    llvm::LLVMContext&
    TypeContext::getLLVMContext() const {
        return m_builder.getContext();
    }
} // namespace ingot::codegen
