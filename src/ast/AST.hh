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

        using const_iterator = std::vector<FunctionDefinition>::const_iterator;
        using iterator = std::vector<FunctionDefinition>::iterator;

        const_iterator begin() const;
        const_iterator end() const;
        iterator begin();
        iterator end();
    };

    std::ostream& operator<<(std::ostream& str, const AST& expr);
} // namespace ingot::ast
