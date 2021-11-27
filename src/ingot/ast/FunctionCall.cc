#include "FunctionCall.hh"

#include <ingot/ast/Expression.hh>
#include <ingot/ast/FunctionDefinition.hh>
#include <ingot/ast/FunctionType.hh>
#include <ingot/Error.hh>

#include <iostream>

namespace ingot::ast
{
    FunctionCall::FunctionCall(std::string name, std::vector<Expression> arguments)
    : Node()
    , m_name(std::move(name))
    , m_arguments(std::move(arguments))
    , m_definition(nullptr /*To be filled in by semantic analyzer*/) {}

    const std::string&
    FunctionCall::getName() const {
        return m_name;
    }

    const std::vector<Expression>&
    FunctionCall::getArguments() const {
        return m_arguments;
    }

    std::vector<Expression>&
    FunctionCall::getArguments() {
        return m_arguments;
    }

    void
    FunctionCall::setFunctionDefinition(const FunctionDefinition& definition) {
        m_definition = &definition;
    }

    const Type&
    FunctionCall::getReturnType() const {
        return getFunctionType().getReturnType(); // :)
    }

    const FunctionType&
    FunctionCall::getFunctionType() const {
        if (!m_definition) {
            throw internal_error("Tried to access function definition before its been set on function call: " + m_name);
        }
        return m_definition->getFunction().getFunctionType();
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
