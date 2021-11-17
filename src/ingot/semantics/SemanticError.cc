#include "SemanticError.hh"

namespace ingot::semantics
{
    SemanticError::SemanticError(const std::string& what, const parser::location& location)
    : std::runtime_error("semantic error: " + what)
    , m_lineno(location.begin.line)
    , m_colno(location.begin.column) {}

    int
    SemanticError::lineno() const {
        return m_lineno;
    }

    int
    SemanticError::colno() const {
        return m_colno;
    }

} // namespace ingot::semantics
