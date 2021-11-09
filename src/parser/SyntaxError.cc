#include "SyntaxError.hh"

namespace ingot::parser
{
    SyntaxError::SyntaxError(const std::string& what, int lineno, int colno)
    : std::runtime_error(what)
    , m_lineno(lineno)
    , m_colno(colno) {}

    int
    SyntaxError::lineno() const {
        return m_lineno;
    }

    int
    SyntaxError::colno() const {
        return m_colno;
    }
} // namespace ingot::parser
