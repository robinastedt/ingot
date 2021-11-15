#pragma once

#include <ingot/ast/AST.hh>
#include <map>

namespace ingot::semantics
{
    class SemanticTree {
        ast::AST m_ast;
        std::map<std::string, const ast::FunctionDefinition&> m_definitionMap;

    public:
        SemanticTree(ast::AST ast);

        void resolve();

        using const_iterator = ast::AST::const_iterator;

        const_iterator begin() const;
        const_iterator end() const;

        const_iterator findDefinition(const std::string& name) const;
    };
} // namespace ingot::semantics
