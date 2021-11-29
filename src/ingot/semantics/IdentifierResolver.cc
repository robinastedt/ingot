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
    IdentifierResolver::postop(ast::Integer& i, std::monostate) { return {}; }

    std::monostate
    IdentifierResolver::postop(ast::Operator& op, const std::pair<std::monostate, std::monostate>&, std::monostate) {
        return {}; 
    }

    std::monostate
    IdentifierResolver::postop(ast::List& list, const std::vector<std::monostate>& elemResults, std::monostate) {
        return {};
    }

    std::monostate
    IdentifierResolver::postop(ast::FunctionCall& func, const std::vector<std::monostate>&, std::monostate) {
        auto defIt = m_definitionMap.find(func.getName());
        if (defIt == m_definitionMap.end()) {
            throw SemanticError("Function not found: '" + func.getName(), func.getLocation());
        }
        const ast::FunctionDefinition& def = defIt->second;
        func.setFunctionDefinition(def);
        return {}; 
    }

    std::monostate
    IdentifierResolver::postop(ast::ArgumentReference& arg, std::monostate) {
        size_t argIndex = m_scopeFunction.findArgumentIndex(arg.getName());
        if (argIndex == -1) {
            throw SemanticError("Argument '" + arg.getName() + "' not found in scope", arg.getLocation());
        }
        const ast::FunctionType& functionType = m_scopeFunction.getFunctionType();
        const ast::Type& argType = functionType.getArgumentType(argIndex);
        arg.resolve(argType, m_scopeFunction, argIndex);
        return {};
    }

    std::monostate
    IdentifierResolver::postop(ast::Ternary& ternary, const std::tuple<std::monostate, std::monostate, std::monostate>& results, std::monostate) {
        return {};
    }

} // namespace ingot::semantics
