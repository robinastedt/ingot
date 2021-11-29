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

    std::monostate
    IdentifierResolver::postop(ast::Integer& i, std::monostate) const { return {}; }

    std::monostate
    IdentifierResolver::postop(ast::String& str, std::monostate) const { return {}; }

    std::monostate
    IdentifierResolver::postop(ast::Operator& op, const std::pair<std::monostate, std::monostate>&, std::monostate) const {
        return {}; 
    }

    std::monostate
    IdentifierResolver::postop(ast::FunctionCall& func, const std::vector<std::monostate>&, std::monostate) const {
        auto defIt = m_definitionMap.find(func.getName());
        if (defIt == m_definitionMap.end()) {
            throw SemanticError("Function not found: '" + func.getName(), func.getLocation());
        }
        const ast::FunctionDefinition& def = defIt->second;
        func.setFunctionDefinition(def);
        return {}; 
    }

    std::monostate
    IdentifierResolver::postop(ast::ArgumentReference& arg, std::monostate) const {
        size_t argIndex = m_scopeFunction.findArgumentIndex(arg.getName());
        if (argIndex == -1) {
            throw SemanticError("Argument '" + arg.getName() + "' not found in scope", arg.getLocation());
        }
        const ast::FunctionType& functionType = m_scopeFunction.getFunctionType();
        const ast::Type& argType = functionType.getArgumentType(argIndex);
        arg.resolve(argType, m_scopeFunction, argIndex);
        return {};
    }


} // namespace ingot::semantics
