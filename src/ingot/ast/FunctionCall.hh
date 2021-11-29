#pragma once

#include <ingot/ast/Type.hh>

#include <string>
#include <vector>
#include <optional>

namespace ingot::ast
{
    class Expression;
    class FunctionDefinition;
    class FunctionType;

    class FunctionCall : public Node {
        std::string m_name;
        std::vector<Expression> m_arguments;
        const FunctionDefinition* m_definition;  // Set by semantic resolver

        friend std::ostream& operator<<(std::ostream& str, const FunctionCall& functionCall);
    public:
        FunctionCall(std::string name, std::vector<Expression> arguments);
        const std::string& getName() const;
        const std::vector<Expression>& getArguments() const;
        std::vector<Expression>& getArguments();
        void setFunctionDefinition(const FunctionDefinition& defintion);
        const Type& getType() const;
        const Type& getReturnType() const;
        const FunctionType& getFunctionType() const;

        bool operator==(const FunctionCall& other) const;
    };

    std::ostream& operator<<(std::ostream& str, const FunctionCall& functionCall);
} // namespace ingot::ast
