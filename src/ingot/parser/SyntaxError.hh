#pragma once

#include <stdexcept>

#include <ingot/parser/location.hh>

namespace ingot::parser
{
    class SyntaxError : public std::runtime_error {
        int m_lineno;
        int m_colno;

    public:
        SyntaxError(const std::string& what, const parser::location& location);
        int lineno() const;
        int colno() const;
    };
} // namespace ingot::parser
