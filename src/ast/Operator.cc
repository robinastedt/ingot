#include "Operator.hh"

#include "Expression.hh"

namespace ingot::ast
{
    Operator::Operator(std::unique_ptr<Expression> lhs, std::unique_ptr<Expression> rhs, Variant variant)
        : m_lhs(std::move(lhs))
        , m_rhs(std::move(rhs))
        , m_variant(variant) {}

    Operator::Operator(const Operator& other)
        : m_lhs(std::make_unique<Expression>(*other.m_lhs))
        , m_rhs(std::make_unique<Expression>(*other.m_rhs))
        , m_variant(other.m_variant) {}

    Operator&
    Operator::operator=(const Operator& other) {
        m_lhs = std::make_unique<Expression>(*other.m_lhs);
        m_rhs = std::make_unique<Expression>(*other.m_rhs);
        m_variant = other.m_variant;
        return *this;
    }

    const Expression&
    Operator::getLhs() const {
        return *m_lhs;
    }

    Expression&
    Operator::getLhs() {
        return *m_lhs;
    }

    const Expression&
    Operator::getRhs() const {
        return *m_rhs;
    }

    Expression&
    Operator::getRhs() {
        return *m_rhs;
    }

    Operator::Variant
    Operator::getVariant() const {
        return m_variant;
    }

    std::ostream& operator<<(std::ostream& str, const Operator& op) {
        return str << "(" << *op.m_lhs << op.m_variant << *op.m_rhs << ")";
    }

    std::ostream& operator<<(std::ostream& str, const Operator::Variant& variant) {
        return str << static_cast<std::underlying_type_t<Operator::Variant>>(variant);
    }
} // namespace ingot::ast
