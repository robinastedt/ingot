#pragma once

#include <memory>
#include <iostream>

namespace ingot::ast
{
    class Expression;

    class Operator {
    public:
        enum class Type : char {
            Add = '+',
            Sub = '-',
            Mul = '*',
            Div = '/',
            Mod = '%',
        };
    private:

        std::unique_ptr<Expression> m_lhs, m_rhs;
        Type m_type;

        friend std::ostream& operator<<(std::ostream& str, const Operator& expr);
        
    public:
        Operator(std::unique_ptr<Expression> lhs, std::unique_ptr<Expression> rhs, Type type);
        Operator(const Operator& other);
        Operator& operator=(const Operator& other);

        const Expression& getLhs() const;
        const Expression& getRhs() const;
        Type getType() const;
    };

    std::ostream& operator<<(std::ostream& str, const Operator& op);
    std::ostream& operator<<(std::ostream& str, const Operator::Type& opType);
} // namespace ingot::ast
