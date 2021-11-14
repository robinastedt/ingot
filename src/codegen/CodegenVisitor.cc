#include "CodegenVisitor.hh"
#include "List.hh"

#include <Error.hh>

#include <llvm/IR/Constants.h>
#include <llvm/IR/Type.h>

#include <boost/range/adaptor/reversed.hpp>

namespace ingot::codegen
{
    CodegenVisitor::CodegenVisitor(
        llvm::Module& module,
        llvm::IRBuilder<>& builder,
        const semantics::SemanticTree& semanticTree,
        const std::map<const ast::FunctionDefinition*, llvm::Function*>& functionMap
    )
    : m_module(module)
    , m_builder(builder)
    , m_i64(m_builder.getInt64Ty())
    , m_semanticTree(semanticTree)
    , m_functionMap(functionMap) {}

    CodegenVisitorInfo
    CodegenVisitor::operator()(const ast::Integer& i) {
        return {llvm::ConstantInt::get(m_i64, i.getValue(), true), i.getType()};
    }

    CodegenVisitorInfo
    CodegenVisitor::operator()(const ast::String& str) {
        using Listi8 = List<char, 15>;
        std::string prefix = str.getType().getSubtype().getName();
        llvm::AllocaInst* listVar = m_builder.CreateAlloca(Listi8::getLLVMType(m_builder, prefix), nullptr, "wrapped_list");
        llvm::Function* fromZeroTermArrayFunction = List<char, 15>::getLLVMFunctionFromZeroTermArray(m_module, m_builder, prefix);
        llvm::Constant* stringConstant = m_builder.CreateGlobalStringPtr(str.getValue());
        m_builder.CreateCall(fromZeroTermArrayFunction, {stringConstant, listVar});
        //llvm::Function* emptyFunction = List<char, 15>::getLLVMFunctionEmpty(m_module, m_builder, prefix);
        //llvm::Function* appendFunction = List<char, 15>::getLLVMFunctionAppend(m_module, m_builder, prefix);
        //m_builder.CreateCall(emptyFunction, {listVar});
        ////llvm::Value* list = emptyList;
        //for (char c : boost::adaptors::reverse(str.getValue())) {
        //    llvm::ConstantInt* charValue = llvm::ConstantInt::get(m_builder.getInt8Ty(), c);
        //    m_builder.CreateCall(appendFunction, {charValue, listVar, listVar});
        //}
        llvm::LoadInst* loadInst = m_builder.CreateLoad(listVar, "load_return");
        return {loadInst, str.getType()};
        //llvm::Value* emptyList = m_builder.CreateCall(m_listi8.getEmptyFunction());
        //llvm::Value* listWithItem = m_builder.CreateCall(m_listi8.getAppendFunction(), {m_builder.getInt8(str.getValue()[0]), emptyList});
        //return {listWithItem, str.getType()};
        //return {m_builder.CreateGlobalStringPtr(str.getValue()), str.getType()}; // TODO: Implement
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
