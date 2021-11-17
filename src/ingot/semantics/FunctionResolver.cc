#include "FunctionResolver.hh"

#include <ingot/Error.hh>

#include <ingot/semantics/SemanticError.hh>
#include <sstream>

namespace ingot::semantics
{
    FunctionResolver::FunctionResolver(const DefMap& definitionMap)
    : m_definitionMap(definitionMap) {}

    void
    FunctionResolver::operator()(ast::Integer& i) {}

    void
    FunctionResolver::operator()(ast::String& str) {}

    void
    FunctionResolver::operator()(ast::Operator& op) {
    }

    void
    FunctionResolver::operator()(ast::FunctionCall& func) {
        auto defIt = m_definitionMap.find(func.getName());
        if (defIt == m_definitionMap.end()) {
            throw SemanticError("Function not found: '" + func.getName(), func.getLocation());
        }
        const ast::FunctionDefinition& def = defIt->second;
        func.setFunctionDefinition(def);
    }

} // namespace ingot::semantics
