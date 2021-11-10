#pragma once

#include <ast/Expression.hh>

#include <semantics/SemanticTree.hh>

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Value.h>
#include <llvm/IR/DerivedTypes.h>

namespace ingot::codegen
{
    class CodegenVisitor : public ast::Expression::Visitor<llvm::Value*> {
        //llvm::LLVMContext& m_context;
        llvm::IRBuilder<>& m_builder;
        llvm::IntegerType* m_i64;
        const semantics::SemanticTree& m_semanticTree;
        const std::map<const ast::FunctionDefinition*, llvm::Function*>& m_functionMap;
    public:
        CodegenVisitor(
            llvm::IRBuilder<>& builder,
            const semantics::SemanticTree& semanticTree,
            const std::map<const ast::FunctionDefinition*, llvm::Function*>& functionMap
        );
        llvm::Value* operator()(const ast::Integer& i) override;
        llvm::Value* operator()(const ast::String& str) override;
        llvm::Value* operator()(const ast::Operator& op, llvm::Value* lhsResult, llvm::Value* rhsResult) override;
        llvm::Value* operator()(const ast::FunctionCall& func) override;
    };
} // namespace ingot::codegen
