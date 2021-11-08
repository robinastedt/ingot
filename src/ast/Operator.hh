#pragma once

#include <memory>
#include <iostream>

namespace ingot::ast
{
    class Expression;

    class Operator {
        std::unique_ptr<Expression> m_lhs, m_rhs;
        char m_op;

        friend std::ostream& operator<<(std::ostream& str, const Operator& expr);

    public:
        Operator(std::unique_ptr<Expression> lhs, std::unique_ptr<Expression> rhs, char op);
        Operator(const Operator& other);
        Operator& operator=(const Operator& other);
    };

    std::ostream& operator<<(std::ostream& str, const Operator& expr);
} // namespace ingot::ast
