#pragma once

#include <memory>
#include <iostream>

namespace ingot::ast
{
    class Expression;

    class Operator {
    public:
        enum class Variant : char {
            Add = '+',
            Sub = '-',
            Mul = '*',
            Div = '/',
            Mod = '%',
        };
    private:

        std::unique_ptr<Expression> m_lhs, m_rhs;
        Variant m_variant;

        friend std::ostream& operator<<(std::ostream& str, const Operator& expr);
        
    public:
        Operator(std::unique_ptr<Expression> lhs, std::unique_ptr<Expression> rhs, Variant variant);
        Operator(const Operator& other);
        Operator& operator=(const Operator& other);

        const Expression& getLhs() const;
        Expression& getLhs();
        const Expression& getRhs() const;
        Expression& getRhs();
        Variant getVariant() const;
    };

    std::ostream& operator<<(std::ostream& str, const Operator& op);
    std::ostream& operator<<(std::ostream& str, const Operator::Variant& variant);
} // namespace ingot::ast