#include "Function.hh"

namespace ingot::ast
{
    Function::Function(FunctionType type, std::vector<std::string> arguments, Expression expression)
    : m_type(std::move(type))
    , m_arguments(std::move(arguments))
    , m_expression(std::move(expression)) {}

    const Expression&
    Function::getExpression() const {
        return m_expression;
    }

    Expression&
    Function::getExpression() {
        return m_expression;
    }

    const FunctionType&
    Function::getFunctionType() const {
        return m_type;
    }

    std::ostream& operator<<(std::ostream& str, const Function& function) {
        str << function.m_type << ":(";
        bool first = true;
        for (const std::string& argument : function.m_arguments) {
            if (!first) {
                str << ",";
            }
            str << argument;
            first = false;
        }
        str << ")->" << function.m_expression;
        return str;
    }

} // namespace ingot::ast

