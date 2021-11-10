#include "CodegenVisitor.hh"

#include <Error.hh>

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

    CodegenVisitorInfo
    CodegenVisitor::operator()(const ast::Integer& i) {
        return {llvm::ConstantInt::get(m_i64, i.getValue(), true), i.getType()};
    }

    CodegenVisitorInfo
    CodegenVisitor::operator()(const ast::String& str) {
        return {m_builder.CreateGlobalStringPtr(str.getValue()), str.getType()}; // TODO: Implement
    }

    CodegenVisitorInfo
    CodegenVisitor::operator()(const ast::Operator& op, CodegenVisitorInfo lhsResult, CodegenVisitorInfo rhsResult) {
        assert(lhsResult.m_type == rhsResult.m_type);
        ast::Type type = lhsResult.m_type;
        if (type == ast::Integer::getType()) {
            switch (op.getVariant()) {
                case ast::Operator::Variant::Add: return {m_builder.CreateAdd(lhsResult.m_value, rhsResult.m_value, "tmpadd"), type};
                case ast::Operator::Variant::Sub: {
                    if (std::holds_alternative<ast::Integer>(op.getLhs()) && std::get<ast::Integer>(op.getLhs()).getValue() == 0) {
                        return {m_builder.CreateNeg(rhsResult.m_value, "tmpneg"), type};
                    }
                    return {m_builder.CreateSub(lhsResult.m_value, rhsResult.m_value, "tmpsub"), type};
                }
                case ast::Operator::Variant::Mul: return { m_builder.CreateMul(lhsResult.m_value, rhsResult.m_value, "tmpmul"), type};
                case ast::Operator::Variant::Div: return { m_builder.CreateSDiv(lhsResult.m_value, rhsResult.m_value, "tmpdiv"), type};
                case ast::Operator::Variant::Mod: return { m_builder.CreateSRem(lhsResult.m_value, rhsResult.m_value, "tmprem"), type};
            }
            throw internal_error(std::string("Unhandled Operator::Variant: ") + (char)(op.getVariant()));
        }
        throw internal_error(std::string("Unhandled type: ") + type.getName());
        
    }

    CodegenVisitorInfo
    CodegenVisitor::operator()(const ast::FunctionCall& funcCall) {
        auto defIt = m_semanticTree.findDefinition(funcCall.getName());
        if (defIt == m_semanticTree.end()) {
            throw internal_error("Could not find function definition in semantic tree: " + funcCall.getName());
        }
        const ast::FunctionDefinition& def = *defIt;
        auto funcIt = m_functionMap.find(&def);
        if (funcIt == m_functionMap.end()) {
            throw internal_error("llvm::Function* not found for function: " + funcCall.getName());
        }
        llvm::Function* func = funcIt->second;
        return {m_builder.CreateCall(func), def.getFunction().getFunctionType().getReturnType()};
    }
} // namespace ingot::codegen
