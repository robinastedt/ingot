#pragma once

#include "FunctionPrototype.hh"
#include "Expression.hh"

namespace ingot::ast
{
    class FunctionDefinition {
        FunctionPrototype m_prototype;
        Expression m_expression;

        friend std::ostream& operator<<(std::ostream& str, const FunctionDefinition& functionDefinition);

    public:
        FunctionDefinition() = default;
        FunctionDefinition(FunctionPrototype prototype, Expression expression);
        const FunctionPrototype& getPrototype() const;
        const Expression& getExpression() const;
    };

    std::ostream& operator<<(std::ostream& str, const FunctionDefinition& functionDefinition);
} // namespace ingot::ast
