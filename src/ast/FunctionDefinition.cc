#include "FunctionDefinition.hh"

namespace ingot::ast
{
    FunctionDefinition::FunctionDefinition(FunctionPrototype prototype, Expression expression)
        : m_prototype(std::move(prototype))
        , m_expression(std::move(expression)) {}

    const FunctionPrototype&
    FunctionDefinition::getPrototype() const {
        return m_prototype;
    }

    const Expression&
    FunctionDefinition::getExpression() const {
        return m_expression;
    }

    bool
    FunctionDefinition::operator<(const FunctionDefinition& rhs) const {
        return m_prototype < rhs.m_prototype;
    }

    std::ostream& operator<<(std::ostream& str, const FunctionDefinition& functionDefinition) {
        return str << functionDefinition.m_prototype << "=" << functionDefinition.m_expression;
    }
} // namespace ingot::ast
