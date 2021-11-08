#pragma once

#include <ast/FunctionDefinition.hh>
#include <vector>

namespace ingot::ast
{
    class AST {
        std::vector<FunctionDefinition> m_functionDefs;

        friend std::ostream& operator<<(std::ostream& str, const AST& expr);
    public:
        AST();
        void addDefinition(FunctionDefinition definition);
    };

    std::ostream& operator<<(std::ostream& str, const AST& expr);
} // namespace ingot::ast
