#include "FunctionDefinition.hh"

namespace ingot::ast
{
    FunctionDefinition::FunctionDefinition(std::string name, Function function)
        : Node()
        , m_name(std::move(name))
        , m_function(std::move(function)) {}

    const std::string&
    FunctionDefinition::getName() const {
        return m_name;
    }

    const Function&
    FunctionDefinition::getFunction() const {
        return m_function;
    }

    Function&
    FunctionDefinition::getFunction() {
        return m_function;
    }

    bool
    FunctionDefinition::operator<(const FunctionDefinition& rhs) const {
        return m_name < rhs.m_name;
    }

    std::ostream& operator<<(std::ostream& str, const FunctionDefinition& functionDefinition) {
        return str << functionDefinition.m_name << "=" << functionDefinition.m_function;
    }
} // namespace ingot::ast
