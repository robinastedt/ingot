#include "AST.hh"

namespace ingot::ast
{
    AST::AST()
    : m_functionDefs() {}

    void
    AST::addDefinition(FunctionDefinition definition) {
        m_functionDefs.emplace_back(std::move(definition));
    }

    AST::const_iterator
    AST::begin() const {
        return m_functionDefs.begin();
    }

    AST::const_iterator
    AST::end() const {
        return m_functionDefs.end();
    }

    AST::iterator
    AST::begin() {
        return m_functionDefs.begin();
    }

    AST::iterator
    AST::end() {
        return m_functionDefs.end();
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
