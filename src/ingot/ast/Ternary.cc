#include "Ternary.hh"

#include <ingot/ast/Expression.hh>

namespace ingot::ast
{
    Ternary::Ternary()
    : m_condition(nullptr)
    , m_trueBranch(nullptr)
    , m_falseBranch(nullptr) {}

    Ternary::Ternary(ExprPtr condition, ExprPtr trueBranch, ExprPtr falseBranch)
    : m_condition(std::move(condition))
    , m_trueBranch(std::move(trueBranch))
    , m_falseBranch(std::move(falseBranch)) {}

    Ternary::Ternary(const Ternary& other)
    : m_condition(std::make_unique<Expression>(*other.m_condition))
    , m_trueBranch(std::make_unique<Expression>(*other.m_trueBranch))
    , m_falseBranch(std::make_unique<Expression>(*other.m_falseBranch)) {}

    Ternary&
    Ternary::operator=(const Ternary& other) {
        m_condition = std::make_unique<Expression>(*other.m_condition);
        m_trueBranch = std::make_unique<Expression>(*other.m_trueBranch);
        m_falseBranch = std::make_unique<Expression>(*other.m_falseBranch);
        return *this;
    }

    Expression&
    Ternary::getCondition() {
        return *m_condition;
    }

    Expression&
    Ternary::getTrueBranch() {
        return *m_trueBranch;
    }

    Expression&
    Ternary::getFalseBranch() {
        return *m_falseBranch;
    }

    const Expression&
    Ternary::getCondition() const {
        return *m_condition;
    }

    const Expression&
    Ternary::getTrueBranch() const {
        return *m_trueBranch;
    }

    const Expression&
    Ternary::getFalseBranch() const {
        return *m_falseBranch;
    }

} // namespace ingot::ast
