#pragma once

#include <ingot/ast/Node.hh>

#include <memory>

namespace ingot::ast
{
    class Expression;

    class Ternary : public Node {
        using ExprPtr = std::unique_ptr<Expression>;
        ExprPtr m_condition;
        ExprPtr m_trueBranch;
        ExprPtr m_falseBranch;

    public:
        Ternary();
        Ternary(ExprPtr condition, ExprPtr trueBranch, ExprPtr falseBranch);
        Ternary(const Ternary& other);
        Ternary& operator=(const Ternary& other);


        Expression& getCondition();
        Expression& getTrueBranch();
        Expression& getFalseBranch();
        const Expression& getCondition() const;
        const Expression& getTrueBranch() const;
        const Expression& getFalseBranch() const;
    };
} // namespace ingot::ast
