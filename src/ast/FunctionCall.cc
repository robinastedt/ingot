#include "FunctionCall.hh"

#include <ast/Expression.hh>

#include <iostream>

namespace ingot::ast
{
    FunctionCall::FunctionCall(std::string name, std::vector<Expression> arguments)
    : m_name(std::move(name))
    , m_arguments(std::move(arguments)) {}

    const std::string&
    FunctionCall::getName() const {
        return m_name;
    }

    const std::vector<Expression>&
    FunctionCall::getArguments() const {
        return m_arguments;
    }

    std::ostream& operator<<(std::ostream& str, const FunctionCall& functionCall) {
        str << functionCall.m_name << "(";
        bool first = true;
        for (const Expression& argument : functionCall.m_arguments) {
            if (!first) {
                str << ",";
            }
            str << argument;
            first = false;
        }
        str << ")";
        return str;
    }
} // namespace ingot::ast
