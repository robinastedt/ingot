#pragma once

#include <stdexcept>
#include <ingot/parser/location.hh>

namespace ingot::semantics
{
    class SemanticError : public std::runtime_error {
        int m_lineno;
        int m_colno;

    public:
        SemanticError(const std::string& what, const parser::location& location);
        int lineno() const;
        int colno() const;
    };
} // namespace ingot::semantics
