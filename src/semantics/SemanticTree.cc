#include "SemanticTree.hh"

#include <semantics/FunctionResolver.hh>
#include <semantics/TypeResolver.hh>
#include <semantics/SemanticError.hh>

#include <sstream>

namespace ingot::semantics
{
    namespace
    {
        using DefMap = std::map<std::string, const ingot::ast::FunctionDefinition&>;
        DefMap
        buildDefinitionMap(const ingot::ast::AST& ast) {
            DefMap defMap;
            for (const ingot::ast::FunctionDefinition& def : ast) {
                defMap.emplace(def.getName(), def);
            }
            return defMap;
        }
    } // namespace
    

    SemanticTree::SemanticTree(ast::AST ast)
    : m_ast(std::move(ast))
    , m_definitionMap(buildDefinitionMap(m_ast)) {}

    void
    SemanticTree::resolve() {
        FunctionResolver functionResolver{m_definitionMap};
        for (ast::FunctionDefinition& def : m_ast) {
            ast::Function& function = def.getFunction();
            ast::Expression& expr = function.getExpression();
            expr.update(functionResolver);
        }

        TypeResolver typeResolver;
        for (ast::FunctionDefinition& def : m_ast) {
            ast::Function& function = def.getFunction();
            ast::Expression& expr = function.getExpression();
            ast::Type exprType = expr.reduce(typeResolver);
            ast::Type retType = function.getFunctionType().getReturnType();
            if (exprType != retType) {
                std::stringstream ss;
                ss << "expression type '" << exprType << "' does not match function's return type: '" << retType << "'";
                throw SemanticError(ss.str());
            }
        }
 
    }

    SemanticTree::const_iterator
    SemanticTree::begin() const {
        return m_ast.begin();
    }

    SemanticTree::const_iterator
    SemanticTree::end() const {
        return m_ast.end();
    }

    SemanticTree::const_iterator
    SemanticTree::findDefinition(const std::string& name) const {
        auto defMapIt = m_definitionMap.find(name);
        return defMapIt != m_definitionMap.end() ? const_iterator{&defMapIt->second} : end();
    }

} // namespace ingot::semantics
