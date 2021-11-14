#pragma once

#include <ast/Expression.hh>
#include <semantics/SemanticTree.hh>
#include <codegen/List.hh>

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Value.h>
#include <llvm/IR/DerivedTypes.h>

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
        const semantics::SemanticTree& m_semanticTree;
        const std::map<const ast::FunctionDefinition*, llvm::Function*>& m_functionMap;
    public:
        CodegenVisitor(
            llvm::Module& module,
            llvm::IRBuilder<>& builder,
            const semantics::SemanticTree& semanticTree,
            const std::map<const ast::FunctionDefinition*, llvm::Function*>& functionMap
        );
        CodegenVisitorInfo operator()(const ast::Integer& i) override;
        CodegenVisitorInfo operator()(const ast::String& str) override;
        CodegenVisitorInfo operator()(const ast::Operator& op, CodegenVisitorInfo lhsResult, CodegenVisitorInfo rhsResult) override;
        CodegenVisitorInfo operator()(const ast::FunctionCall& func) override;
    };
} // namespace ingot::codegen
