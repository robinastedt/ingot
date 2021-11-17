#include "SyntaxError.hh"

namespace ingot::parser
{
    SyntaxError::SyntaxError(const std::string& what, const parser::location& location)
    : std::runtime_error(what)
    , m_lineno(location.begin.line)
    , m_colno(location.begin.column) {}

    int
    SyntaxError::lineno() const {
        return m_lineno;
    }

    int
    SyntaxError::colno() const {
        return m_colno;
    }
} // namespace ingot::parser
