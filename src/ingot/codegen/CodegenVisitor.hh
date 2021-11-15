#pragma once

#include <ingot/ast/Expression.hh>
#include <ingot/semantics/SemanticTree.hh>
#include <ingot/codegen/ListOperations.hh>

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Value.h>

namespace ingot::codegen
{
    struct CodegenVisitorInfo {
        llvm::Value* m_value;
        ast::Type m_type;
    };

    class CodegenVisitor : public ast::Expression::Visitor<CodegenVisitorInfo> {
        llvm::Module& m_module;
        llvm::IRBuilder<>& m_builder;
        llvm::IntegerType* m_i64;
        TypeContext& m_typeContext;
        ListOperations::Collection& m_listOperationsCollection;
        const semantics::SemanticTree& m_semanticTree;
        const std::map<const ast::FunctionDefinition*, llvm::Function*>& m_functionMap;
    public:
        CodegenVisitor(
            TypeContext& typeContext,
            ListOperations::Collection& listOperationsCollection,
            const semantics::SemanticTree& semanticTree,
            const std::map<const ast::FunctionDefinition*, llvm::Function*>& functionMap
        );
        CodegenVisitorInfo operator()(const ast::Integer& i) override;
        CodegenVisitorInfo operator()(const ast::String& str) override;
        CodegenVisitorInfo operator()(const ast::Operator& op, CodegenVisitorInfo lhsResult, CodegenVisitorInfo rhsResult) override;
        CodegenVisitorInfo operator()(const ast::FunctionCall& func) override;
    };
} // namespace ingot::codegen
