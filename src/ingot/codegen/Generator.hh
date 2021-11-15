#pragma once

#include <llvm/IR/LLVMContext.h>

namespace ingot::semantics
{
    class SemanticTree;
} // namespace ingot::ast


namespace ingot::codegen
{
    class Generator {
        llvm::LLVMContext m_context;

        const std::string userFunctionPrefix = "ingot_user_";

    public:
        Generator();
        void run(const semantics::SemanticTree& semTree);
    };
} // namespace ingot::codegen
