#pragma once

#include <ingot/ast/Expression.hh>
#include <ingot/semantics/SemanticTree.hh>
#include <ingot/codegen/ListOperations.hh>

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Value.h>

#include <stack>

namespace ingot::codegen
{
    struct CodegenVisitorInfo {
        llvm::BasicBlock* m_block;
        llvm::Value* m_value;
        ast::Type m_type;
    };

    class CodegenVisitor : public ast::Expression::ConstVisitor<CodegenVisitorInfo, std::monostate> {

        struct TernaryContext {
            llvm::BasicBlock* entry;
            llvm::BasicBlock* trueBranch;
            llvm::BasicBlock* falseBranch;
            llvm::BasicBlock* result;
        };

        llvm::Module& m_module;
        llvm::IRBuilder<>& m_builder;
        llvm::Function* m_scopeFunction;
        llvm::IntegerType* m_i64;
        TypeContext& m_typeContext;
        ListOperations::Collection& m_listOperationsCollection;
        const semantics::SemanticTree& m_semanticTree;
        const std::map<const ast::FunctionDefinition*, llvm::Function*>& m_functionMap;
        mutable std::stack<TernaryContext> m_ternaryContextStack;
    public:
        CodegenVisitor(
            llvm::Function* scopeFunction,
            TypeContext& typeContext,
            ListOperations::Collection& listOperationsCollection,
            const semantics::SemanticTree& semanticTree,
            const std::map<const ast::FunctionDefinition*, llvm::Function*>& functionMap
        );
        std::monostate preop(const ast::Ternary& func, std::monostate input, TernaryPosition position) const override;
        CodegenVisitorInfo postop(const ast::Integer& i, std::monostate) const override;
        CodegenVisitorInfo postop(const ast::List& list, const std::vector<CodegenVisitorInfo>& results, std::monostate) const override;
        CodegenVisitorInfo postop(const ast::Operator& op, const std::pair<CodegenVisitorInfo,CodegenVisitorInfo>& results, std::monostate) const override;
        CodegenVisitorInfo postop(const ast::FunctionCall& func, const std::vector<CodegenVisitorInfo>& results, std::monostate) const override;
        CodegenVisitorInfo postop(const ast::ArgumentReference& arg, std::monostate) const override;
        CodegenVisitorInfo postop(const ast::Ternary& ternary, const std::tuple<CodegenVisitorInfo, CodegenVisitorInfo, CodegenVisitorInfo>& results, std::monostate) const override;
    };
} // namespace ingot::codegen
