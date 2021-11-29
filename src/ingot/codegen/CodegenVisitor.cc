#include "CodegenVisitor.hh"

#include <ingot/Error.hh>

#include <llvm/IR/Constants.h>
#include <llvm/IR/Type.h>

#include <boost/range/adaptor/reversed.hpp>

namespace ingot::codegen
{
    CodegenVisitor::CodegenVisitor(
        llvm::Function* scopeFunction,
        TypeContext& typeContext,
        ListOperations::Collection& listOperationsCollection,
        const semantics::SemanticTree& semanticTree,
        const std::map<const ast::FunctionDefinition*, llvm::Function*>& functionMap
    )
    : m_module(typeContext.getModule())
    , m_builder(typeContext.getBuilder())
    , m_scopeFunction(scopeFunction)
    , m_i64(m_builder.getInt64Ty())
    , m_typeContext(typeContext)
    , m_listOperationsCollection(listOperationsCollection)
    , m_semanticTree(semanticTree)
    , m_functionMap(functionMap)
    , m_ternaryContextStack() {}

    std::monostate
    CodegenVisitor::preop(const ast::Ternary& func, std::monostate input, TernaryPosition position) {
        switch (position) {
            case TernaryPosition::CONDITION: {
                std::string twineSuffix = std::to_string(m_ternaryContextStack.size());
                TernaryContext& context = m_ternaryContextStack.emplace();
                context.entry = m_builder.GetInsertBlock();
                context.trueBranch = llvm::BasicBlock::Create(m_builder.getContext(), "trueBranch_" + twineSuffix, m_scopeFunction);
                context.falseBranch = llvm::BasicBlock::Create(m_builder.getContext(), "falseBranch_" + twineSuffix, m_scopeFunction);
                context.result = llvm::BasicBlock::Create(m_builder.getContext(), "result_" + twineSuffix, m_scopeFunction);
            } break;
            case TernaryPosition::TRUE_BRANCH: {
                m_builder.SetInsertPoint(m_ternaryContextStack.top().trueBranch);
            } break;
            case TernaryPosition::FALSE_BRANCH: {
                m_builder.SetInsertPoint(m_ternaryContextStack.top().falseBranch);
            }
        }
        return {};
    }

    CodegenVisitorInfo
    CodegenVisitor::postop(const ast::Integer& i, std::monostate) {
        return {m_builder.GetInsertBlock(), llvm::ConstantInt::get(m_builder.getIntNTy(i.getType().getSize()), i.getValue()), i.getType()};
    }

    CodegenVisitorInfo
    CodegenVisitor::postop(const ast::List& list, const std::vector<CodegenVisitorInfo>& results, std::monostate) {
        const ListOperations& listOperations = m_listOperationsCollection.get(list.getType());
        llvm::Function* emptyFunction = listOperations.getEmptyFunction();
        llvm::Function* appendFunction = listOperations.getAppendFunction();
        llvm::Type* listType = m_typeContext.getLLVMType(list.getType());
        llvm::Value* emptyList = m_builder.CreateCall(emptyFunction);
        llvm::AllocaInst* listPtr = m_builder.CreateAlloca(listType, nullptr, "listPtr");
        m_builder.CreateStore(emptyList, listPtr);
        // TODO: This is extremely inefficient, implement function to initialize list from array instead
        for (const CodegenVisitorInfo& elemInfo : boost::adaptors::reverse(results)) {
            llvm::Value* list = m_builder.CreateCall(appendFunction, {elemInfo.m_value, listPtr});
            m_builder.CreateStore(list, listPtr);
        }
        llvm::LoadInst* loadInst = m_builder.CreateLoad(listPtr, "load_return");
        return {m_builder.GetInsertBlock(), loadInst, list.getType()};
    }

    CodegenVisitorInfo
    CodegenVisitor::postop(const ast::Operator& op, const std::pair<CodegenVisitorInfo, CodegenVisitorInfo> &results, std::monostate) {
        const CodegenVisitorInfo& lhsResult = results.first;
        const CodegenVisitorInfo& rhsResult = results.second;
        assert(lhsResult.m_type == rhsResult.m_type);
        ast::Type type = lhsResult.m_type;
        if (type.getVariant() == ast::Type::Variant::Integer) {
            switch (op.getVariant()) {
                case ast::Operator::Variant::Add: return {m_builder.GetInsertBlock(), m_builder.CreateAdd(lhsResult.m_value, rhsResult.m_value, "tmpadd"), type};
                case ast::Operator::Variant::Sub: {
                    if (std::holds_alternative<ast::Integer>(op.getLhs()) && std::get<ast::Integer>(op.getLhs()).getValue() == 0) {
                        return {m_builder.GetInsertBlock(), m_builder.CreateNeg(rhsResult.m_value, "tmpneg"), type};
                    }
                    return {m_builder.GetInsertBlock(), m_builder.CreateSub(lhsResult.m_value, rhsResult.m_value, "tmpsub"), type};
                }
                case ast::Operator::Variant::Mul: return { m_builder.GetInsertBlock(), m_builder.CreateMul(lhsResult.m_value, rhsResult.m_value, "tmpmul"), type};
                case ast::Operator::Variant::Div: return { m_builder.GetInsertBlock(), m_builder.CreateSDiv(lhsResult.m_value, rhsResult.m_value, "tmpdiv"), type};
                case ast::Operator::Variant::Mod: return { m_builder.GetInsertBlock(), m_builder.CreateSRem(lhsResult.m_value, rhsResult.m_value, "tmprem"), type};
            }
            throw internal_error("Unhandled Operator::Variant: " + ast::Operator::variantToString(op.getVariant()));
        }
        throw internal_error(std::string("Unhandled type: ") + type.getName());
        
    }

    CodegenVisitorInfo
    CodegenVisitor::postop(const ast::FunctionCall& funcCall, const std::vector<CodegenVisitorInfo>& results, std::monostate) {
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

        std::vector<llvm::Value*> argValues;
        for (const CodegenVisitorInfo& result : results) {
            argValues.push_back(result.m_value);
        }

        return {m_builder.GetInsertBlock(), m_builder.CreateCall(func, argValues), def.getFunction().getFunctionType().getReturnType()};
    }

    CodegenVisitorInfo
    CodegenVisitor::postop(const ast::ArgumentReference& arg, std::monostate) {
        return {m_builder.GetInsertBlock(), m_scopeFunction->getArg(arg.getIndex()), arg.getType()};
    }

    CodegenVisitorInfo
    CodegenVisitor::postop(const ast::Ternary& ternary, const std::tuple<CodegenVisitorInfo, CodegenVisitorInfo, CodegenVisitorInfo>& results, std::monostate) {
        const CodegenVisitorInfo& condInfo = std::get<0>(results);
        const CodegenVisitorInfo& trueBranchInfo = std::get<1>(results);
        const CodegenVisitorInfo& falseBranchInfo = std::get<2>(results);

        if (condInfo.m_type != ast::Type::integer(1)) {
            throw internal_error("Condition is not boolean type.");
        }
        if (trueBranchInfo.m_type != falseBranchInfo.m_type) {
            throw internal_error("True and false branch types do not match.");
        }

        TernaryContext context = m_ternaryContextStack.top();
        m_ternaryContextStack.pop();
        m_builder.SetInsertPoint(context.entry);
        m_builder.CreateCondBr(condInfo.m_value, context.trueBranch, context.falseBranch);
        m_builder.SetInsertPoint(trueBranchInfo.m_block);
        m_builder.CreateBr(context.result);
        m_builder.SetInsertPoint(falseBranchInfo.m_block);
        m_builder.CreateBr(context.result);
        m_builder.SetInsertPoint(context.result);
        llvm::PHINode* phiNode = m_builder.CreatePHI(m_typeContext.getLLVMType(trueBranchInfo.m_type), 2, "ternaryResult");
        phiNode->addIncoming(trueBranchInfo.m_value, trueBranchInfo.m_block);
        phiNode->addIncoming(falseBranchInfo.m_value, falseBranchInfo.m_block);
        return {context.result, phiNode, trueBranchInfo.m_type};
    }
} // namespace ingot::codegen
