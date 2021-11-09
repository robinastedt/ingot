#include "SemanticTree.hh"

namespace ingot::semantics
{
    namespace
    {
        using DefMap = std::map<ingot::ast::FunctionPrototype, const ingot::ast::FunctionDefinition&>;
        DefMap
        buildDefinitionMap(const ingot::ast::AST& ast) {
            DefMap defMap;
            for (const ingot::ast::FunctionDefinition& def : ast) {
                defMap.emplace(def.getPrototype(), def);
            }
            return defMap;
        }
    } // namespace
    

    SemanticTree::SemanticTree(const ast::AST& ast)
    : m_ast(ast)
    , m_definitionMap(buildDefinitionMap(m_ast)) {}

    bool
    SemanticTree::verify(std::ostream& err) const {
        // TODO: Implement when function calls are available
        return true;
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
    SemanticTree::findDefinition(const ast::FunctionPrototype& prototype) const {
        auto defMapIt = m_definitionMap.find(prototype);
        return defMapIt != m_definitionMap.end() ? const_iterator{&defMapIt->second} : end();
    }

} // namespace ingot::semantics
