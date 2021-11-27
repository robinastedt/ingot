#pragma once

#include <ingot/ast/Expression.hh>

#include <map>
#include <string>

namespace ingot::semantics
{
    class IdentifierResolver : public ast::Expression::UpdateVisitor {
        using DefMap = std::map<std::string, const ast::FunctionDefinition&>;

        const ast::Function& m_scopeFunction;
        const DefMap& m_definitionMap;
    public:
        IdentifierResolver(const ast::Function& scopeFunction, const DefMap& definitionMap);

        void operator()(ast::Integer& i) override;
        void operator()(ast::String& str) override;
        void operator()(ast::Operator& op) override;
        void operator()(ast::FunctionCall& func) override;
        void operator()(ast::ArgumentReference& arg) override;
    };
} // namespace ingot::semantics
