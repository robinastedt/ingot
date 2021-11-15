#pragma once

#include <ast/Type.hh>

#include <llvm/IR/IRBuilder.h>
#include <map>

namespace ingot::codegen
{
    class TypeContext {
        llvm::IRBuilder<>& m_builder;
        llvm::Module& m_module;
        std::map<ast::Type, llvm::Type*> m_typeMap;
        std::map<ast::Type, llvm::ArrayType*> m_typeMapListArrayMember;
    public:
        TypeContext(llvm::IRBuilder<>& builder, llvm::Module& m_module);
        llvm::Type* getLLVMType(const ast::Type& type);
        llvm::ArrayType* getLLVMTypeForListArrayMember(const ast::Type& elementType);
        llvm::IntegerType* getLLVMTypeForListCounterMember();
        llvm::Module& getModule() const;
        llvm::IRBuilder<>& getBuilder() const;
        llvm::LLVMContext& getLLVMContext() const;

        
    };
} // namespace ingot::codegen
