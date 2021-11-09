#include "Operator.hh"

#include "Expression.hh"

namespace ingot::ast
{
    Operator::Operator(std::unique_ptr<Expression> lhs, std::unique_ptr<Expression> rhs, Type type)
        : m_lhs(std::move(lhs))
        , m_rhs(std::move(rhs))
        , m_type(type) {}

    Operator::Operator(const Operator& other)
        : m_lhs(std::make_unique<Expression>(*other.m_lhs))
        , m_rhs(std::make_unique<Expression>(*other.m_rhs))
        , m_type(other.m_type) {}

    Operator&
    Operator::operator=(const Operator& other) {
        m_lhs = std::make_unique<Expression>(*other.m_lhs);
        m_rhs = std::make_unique<Expression>(*other.m_rhs);
        m_type = other.m_type;
        return *this;
    }

    const Expression&
    Operator::getLhs() const {
        return *m_lhs;
    }

    const Expression&
    Operator::getRhs() const {
        return *m_rhs;
    }

    Operator::Type
    Operator::getType() const {
        return m_type;
    }

    std::ostream& operator<<(std::ostream& str, const Operator& op) {
        return str << "(" << *op.m_lhs << op.m_type << *op.m_rhs << ")";
    }

    std::ostream& operator<<(std::ostream& str, const Operator::Type& opType) {
        return str << static_cast<std::underlying_type_t<Operator::Type>>(opType);
    }
} // namespace ingot::ast
