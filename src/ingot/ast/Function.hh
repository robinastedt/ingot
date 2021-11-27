#pragma once

#include <ingot/ast/FunctionType.hh>
#include <ingot/ast/Expression.hh>

#include <vector>
#include <string>

namespace ingot::ast
{
    class Function : public Node {
        ast::FunctionType m_type;
        std::vector<std::string> m_arguments;
        ast::Expression m_expression;

        friend std::ostream& operator<<(std::ostream& str, const Function& function);
    public:
        Function() = default;
        Function(FunctionType type, std::vector<std::string> arguments, ast::Expression expression);

        const Expression& getExpression() const;
        Expression& getExpression();
        const FunctionType& getFunctionType() const;
        size_t findArgumentIndex(const std::string& name) const;
    };

    std::ostream& operator<<(std::ostream& str, const Function& function);
} // namespace ingot::ast
