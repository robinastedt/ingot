#pragma once

#include "Function.hh"

#include <string>

namespace ingot::ast
{
    class FunctionDefinition {
        std::string m_name;
        Function m_function;

        friend std::ostream& operator<<(std::ostream& str, const FunctionDefinition& functionDefinition);

    public:
        FunctionDefinition() = default;
        FunctionDefinition(std::string name, Function function);
        const std::string& getName() const;
        const Function& getFunction() const;
        Function& getFunction();

        bool operator<(const FunctionDefinition& rhs) const;
    };

    std::ostream& operator<<(std::ostream& str, const FunctionDefinition& functionDefinition);
} // namespace ingot::ast
