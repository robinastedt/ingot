#pragma once

#include <ast/AST.hh>
#include <map>

namespace ingot::semantics
{
    class SemanticTree {
        const ast::AST& m_ast;
        std::map<std::string, const ast::FunctionDefinition&> m_definitionMap;

    public:
        SemanticTree(const ast::AST& ast);

        bool verify(std::ostream& err) const;

        using const_iterator = ast::AST::const_iterator;

        const_iterator begin() const;
        const_iterator end() const;

        const_iterator findDefinition(const std::string& name) const;
    };
} // namespace ingot::semantics
