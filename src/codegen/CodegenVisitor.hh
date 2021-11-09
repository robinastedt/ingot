#pragma once

#include <ast/Expression.hh>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Value.h>
#include <llvm/IR/DerivedTypes.h>

namespace ingot::codegen
{
    class CodegenVisitor : public ast::Expression::Visitor<llvm::Value*> {
        //llvm::LLVMContext& m_context;
        llvm::IRBuilder<>& m_builder;
        llvm::IntegerType* m_i64;
    public:
        CodegenVisitor(llvm::IRBuilder<>& builder);
        llvm::Value* operator()(const ast::Integer& i) override;
        llvm::Value* operator()(const ast::Operator& op, llvm::Value* lhsResult, llvm::Value* rhsResult) override;
    };
} // namespace ingot::codegen
