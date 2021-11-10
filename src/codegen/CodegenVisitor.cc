#include "CodegenVisitor.hh"

#include <llvm/IR/Constants.h>
#include <llvm/IR/Type.h>

namespace ingot::codegen
{
    CodegenVisitor::CodegenVisitor(
        llvm::IRBuilder<>& builder,
        const semantics::SemanticTree& semanticTree,
        const std::map<const ast::FunctionDefinition*, llvm::Function*>& functionMap
    )
    : m_builder(builder)
    , m_i64(m_builder.getInt64Ty())
    , m_semanticTree(semanticTree)
    , m_functionMap(functionMap) {}

    llvm::Value*
    CodegenVisitor::operator()(const ast::Integer& i) {
        return llvm::ConstantInt::get(m_i64, i.getValue(), true);
    }

    llvm::Value*
    CodegenVisitor::operator()(const ast::Operator& op, llvm::Value* lhsResult, llvm::Value* rhsResult) {
        switch (op.getType()) {
            case ast::Operator::Type::Add: return m_builder.CreateAdd(lhsResult, rhsResult, "tmpadd");
            case ast::Operator::Type::Sub: {
                if (std::holds_alternative<ast::Integer>(op.getLhs()) && std::get<ast::Integer>(op.getLhs()).getValue() == 0) {
                    return m_builder.CreateNeg(rhsResult, "tmpneg");
                }
                return m_builder.CreateSub(lhsResult, rhsResult, "tmpsub");
            }
            case ast::Operator::Type::Mul: return m_builder.CreateMul(lhsResult, rhsResult, "tmpmul");
            case ast::Operator::Type::Div: return m_builder.CreateSDiv(lhsResult, rhsResult, "tmpdiv");
            case ast::Operator::Type::Mod: return m_builder.CreateSRem(lhsResult, rhsResult, "tmprem");
        } 
        throw std::runtime_error(std::string("Internal error: Unhandled Operator::Type: ") + (char)(op.getType()));
    }

    llvm::Value*
    CodegenVisitor::operator()(const ast::FunctionCall& funcCall) {
        auto defIt = m_semanticTree.findDefinition(funcCall.getName());
        if (defIt == m_semanticTree.end()) {
            throw std::runtime_error("Internal error: Could not find function definition in semantic tree: " + funcCall.getName());
        }
        const ast::FunctionDefinition& def = *defIt;
        auto funcIt = m_functionMap.find(&def);
        if (funcIt == m_functionMap.end()) {
            throw std::runtime_error("Internal error: llvm::Function* not found for function: " + funcCall.getName());
        }
        llvm::Function* func = funcIt->second;
        return m_builder.CreateCall(func);
    }
} // namespace ingot::codegen
