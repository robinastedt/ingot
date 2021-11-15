#include "SemanticError.hh"

namespace ingot::semantics
{
    SemanticError::SemanticError(const std::string& what)
    : std::runtime_error("semantic error: " + what) {}

} // namespace ingot::semantics
