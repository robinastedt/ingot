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

    class CodegenVisitor : public ast::Expression::ConstVisitor<CodegenVisitorInfo, std::monostate> {
        llvm::Module& m_module;
        llvm::IRBuilder<>& m_builder;
        llvm::Function* m_scopeFunction;
        llvm::IntegerType* m_i64;
        TypeContext& m_typeContext;
        ListOperations::Collection& m_listOperationsCollection;
        const semantics::SemanticTree& m_semanticTree;
        const std::map<const ast::FunctionDefinition*, llvm::Function*>& m_functionMap;
    public:
        CodegenVisitor(
            llvm::Function* scopeFunction,
            TypeContext& typeContext,
            ListOperations::Collection& listOperationsCollection,
            const semantics::SemanticTree& semanticTree,
            const std::map<const ast::FunctionDefinition*, llvm::Function*>& functionMap
        );
        CodegenVisitorInfo postop(const ast::Integer& i, std::monostate) const override;
        CodegenVisitorInfo postop(const ast::String& str, std::monostate) const override;
        CodegenVisitorInfo postop(const ast::Operator& op, const std::pair<CodegenVisitorInfo,CodegenVisitorInfo>& results, std::monostate) const override;
        CodegenVisitorInfo postop(const ast::FunctionCall& func, const std::vector<CodegenVisitorInfo>& results, std::monostate) const override;
        CodegenVisitorInfo postop(const ast::ArgumentReference& arg, std::monostate) const override;
    };
} // namespace ingot::codegen
