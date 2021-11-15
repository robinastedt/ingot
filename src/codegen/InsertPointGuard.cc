#include "InsertPointGuard.hh"

namespace ingot::codegen
{
    InsertPointGuard::InsertPointGuard(llvm::IRBuilder<>& builder)
    : m_builder(builder)
    , m_insertBlock(m_builder.GetInsertBlock()) {}
    
    InsertPointGuard::~InsertPointGuard() {
        m_builder.SetInsertPoint(m_insertBlock);
    }
} // namespace ingot::codegen
