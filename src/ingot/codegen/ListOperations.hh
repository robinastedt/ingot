#pragma once

#include <ingot/ast/Type.hh>

#include <ingot/codegen/TypeContext.hh>

namespace ingot::codegen
{
    class ListOperations {
        ast::Type m_type;
        ast::Type m_subtype;
        llvm::Type* m_llvmType;
        llvm::Function* m_llvmFuncEmpty;
        llvm::Function* m_llvmFuncIsEmpty;
        llvm::Function* m_llvmFuncAppend;
        llvm::Function* m_llvmFuncHead;
        llvm::Function* m_llvmFuncTail;
        
        ListOperations(TypeContext& typeContext, ast::Type type);
    public:
        class Collection {
            TypeContext& m_typeContext;
            std::map<ast::Type, ListOperations> m_listOperationsMap;

        public:
            Collection(TypeContext& typeContext);
            const ListOperations& get(const ast::Type& type);
        };

        // () -> list
        llvm::Function* getEmptyFunction() const;
        // (list*) -> bool
        llvm::Function* getIsEmptyFunction() const;
        // (element, list*) -> list
        llvm::Function* getAppendFunction() const;
        // (list*) -> element
        llvm::Function* getHeadFunction() const;
        // (list*) -> list
        llvm::Function* getTailFunction() const;
    };
} // namespace ingot::codegen
