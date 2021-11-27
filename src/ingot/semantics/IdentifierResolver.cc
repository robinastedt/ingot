#include "IdentifierResolver.hh"

#include <ingot/Error.hh>

#include <ingot/ast/Function.hh>

#include <ingot/semantics/SemanticError.hh>
#include <sstream>

namespace ingot::semantics
{
    IdentifierResolver::IdentifierResolver(const ast::Function& scopeFunction, const DefMap& definitionMap)
    : m_scopeFunction(scopeFunction)
    , m_definitionMap(definitionMap) {}

    void
    IdentifierResolver::operator()(ast::Integer& i) {}

    void
    IdentifierResolver::operator()(ast::String& str) {}

    void
    IdentifierResolver::operator()(ast::Operator& op) {
    }

    void
    IdentifierResolver::operator()(ast::FunctionCall& func) {
        auto defIt = m_definitionMap.find(func.getName());
        if (defIt == m_definitionMap.end()) {
            throw SemanticError("Function not found: '" + func.getName(), func.getLocation());
        }
        const ast::FunctionDefinition& def = defIt->second;
        func.setFunctionDefinition(def);
    }

    void
    IdentifierResolver::operator()(ast::ArgumentReference& arg) {
        size_t argIndex = m_scopeFunction.findArgumentIndex(arg.getName());
        if (argIndex == -1) {
            throw SemanticError("Argument '" + arg.getName() + "' not found in scope", arg.getLocation());
        }
        const ast::FunctionType& functionType = m_scopeFunction.getFunctionType();
        const ast::Type& argType = functionType.getArgumentType(argIndex);
        arg.resolve(argType, m_scopeFunction, argIndex);
    }

} // namespace ingot::semantics
