#include "AST.hh"

namespace ingot::ast
{
    AST::AST()
    : m_functionDefs() {}

    void
    AST::addDefinition(FunctionDefinition definition) {
        m_functionDefs.emplace_back(std::move(definition));
    }

    std::ostream& operator<<(std::ostream& str, const AST& expr) {
        bool first = true;
        for (const FunctionDefinition& def : expr.m_functionDefs) {
            if (!first) {
                str << "\n";
            }
            str << def;
            first = false;
        }
        return str;
    }
} // namespace ingot::ast
