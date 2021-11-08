#include "Operator.hh"

#include "Expression.hh"

namespace ingot::ast
{
    Operator::Operator(std::unique_ptr<Expression> lhs, std::unique_ptr<Expression> rhs, char op)
        : m_lhs(std::move(lhs))
        , m_rhs(std::move(rhs))
        , m_op(op) {}

    Operator::Operator(const Operator& other)
        : m_lhs(std::make_unique<Expression>(*other.m_lhs))
        , m_rhs(std::make_unique<Expression>(*other.m_rhs))
        , m_op(other.m_op) {}

    Operator&
    Operator::operator=(const Operator& other) {
        m_lhs = std::make_unique<Expression>(*other.m_lhs);
        m_rhs = std::make_unique<Expression>(*other.m_rhs);
        m_op = other.m_op;
        return *this;
    }


    std::ostream& operator<<(std::ostream& str, const Operator& expr) {
        return str << "(" << *expr.m_lhs << expr.m_op << *expr.m_rhs << ")";
    }
} // namespace ingot::ast
