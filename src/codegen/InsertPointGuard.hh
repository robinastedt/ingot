#pragma once

#include <llvm/IR/IRBuilder.h>

namespace ingot::codegen
{
    // Resets the current insert point when guard goes out of scope
    class InsertPointGuard {
        llvm::IRBuilder<>& m_builder;
        llvm::BasicBlock* m_insertBlock;

    public:
        InsertPointGuard(llvm::IRBuilder<>& builder);
        ~InsertPointGuard();
    };
} // namespace ingot::codegen
