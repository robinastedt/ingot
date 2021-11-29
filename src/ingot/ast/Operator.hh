#pragma once

#include <memory>
#include <iostream>

#include <ingot/ast/Node.hh>

namespace ingot::ast
{
    class Expression;

    class Operator : public Node {
    public:
        enum class Variant {
            Add,
            Sub,
            Mul,
            Div,
            Mod
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

        static std::string variantToString(Variant var);
    };

    std::ostream& operator<<(std::ostream& str, const Operator& op);
    std::ostream& operator<<(std::ostream& str, const Operator::Variant& variant);
} // namespace ingot::ast
