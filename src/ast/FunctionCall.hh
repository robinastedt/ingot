#pragma once

#include <string>
#include <vector>

namespace ingot::ast
{
    class Expression;

    class FunctionCall {
        std::string m_name;
        std::vector<Expression> m_arguments;

        friend std::ostream& operator<<(std::ostream& str, const FunctionCall& functionCall);
    public:
        FunctionCall(std::string name, std::vector<Expression> arguments);
        const std::string& getName() const;
        const std::vector<Expression>& getArguments() const;
    };

    std::ostream& operator<<(std::ostream& str, const FunctionCall& functionCall);
} // namespace ingot::ast
