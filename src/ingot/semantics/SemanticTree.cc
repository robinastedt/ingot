#include "SemanticTree.hh"

#include <ingot/semantics/IdentifierResolver.hh>
#include <ingot/semantics/TypeResolver.hh>
#include <ingot/semantics/SemanticError.hh>

#include <sstream>
#include <set>

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
        
        // Check multiple function declarations
        std::map<std::string, const ast::FunctionDefinition*> seenFunctions;
        for (ast::FunctionDefinition& def : m_ast) {
            if (auto it = seenFunctions.find(def.getName()); it != seenFunctions.end()) {
                const ast::FunctionDefinition& prevDef = *it->second;
                const auto& prevLoc = prevDef.getLocation();
                std::stringstream ss;
                ss << "Function '" << def.getName() << "' "
                   << "already declared at line " << prevLoc.begin.line
                   << ", column " << prevLoc.begin.column;
                throw SemanticError(ss.str(), def.getLocation());
            }
            seenFunctions.emplace(std::make_pair(def.getName(), &def));
        }

        // Resolve indentifiers
        // Update pointers in ArgumentReference and FunctionCall
        for (ast::FunctionDefinition& def : m_ast) {
            ast::Function& function = def.getFunction();
            ast::Expression& expr = function.getExpression();
            IdentifierResolver identifierResolver{function, m_definitionMap};
            expr.traverse(identifierResolver);
        }

        // Resolve size of integer constants and check that types match
        TypeResolver typeResolver;
        for (ast::FunctionDefinition& def : m_ast) {
            ast::Function& function = def.getFunction();
            ast::Expression& expr = function.getExpression();
            ast::Type retType = function.getFunctionType().getReturnType();
            size_t retSize = retType.getVariant() == ast::Type::Variant::Integer ? retType.getSize() : 0;
            ast::Type exprType = expr.traverse(typeResolver, retSize);
            
            if (exprType != retType) {
                std::stringstream ss;
                ss << "expression '" << expr << "' of type '" << exprType << "' does not match function's return type: '" << retType << "'";
                throw SemanticError(ss.str(), expr.getLocation());
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

    std::ostream& operator<<(std::ostream& str, const SemanticTree& tree) {
        return str << tree.m_ast;
    }

} // namespace ingot::semantics
