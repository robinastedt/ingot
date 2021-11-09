#pragma once

#include <stdexcept>

namespace ingot::parser
{
    class SyntaxError : public std::runtime_error {
        int m_lineno;
        int m_colno;

    public:
        SyntaxError(const std::string& what, int lineno, int colno);
        int lineno() const;
        int colno() const;
    };
} // namespace ingot::parser
