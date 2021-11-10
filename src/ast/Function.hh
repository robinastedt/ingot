#pragma once

#include <ast/FunctionType.hh>
#include <ast/Expression.hh>

#include <vector>
#include <string>

namespace ingot::ast
{
    class Function {
        ast::FunctionType m_type;
        std::vector<std::string> m_arguments;
        ast::Expression m_expression;

        friend std::ostream& operator<<(std::ostream& str, const Function& function);
    public:
        Function() = default;
        Function(FunctionType type, std::vector<std::string> arguments, ast::Expression expression);

        const ast::Expression& getExpression() const;
    };

    std::ostream& operator<<(std::ostream& str, const Function& function);
} // namespace ingot::ast
