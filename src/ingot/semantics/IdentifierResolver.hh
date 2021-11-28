#pragma once

#include <ingot/ast/Expression.hh>

#include <map>
#include <string>

namespace ingot::semantics
{
    class IdentifierResolver : public ast::Expression::UpdateVisitor<> {
        using DefMap = std::map<std::string, const ast::FunctionDefinition&>;

        const ast::Function& m_scopeFunction;
        const DefMap& m_definitionMap;
    public:
        IdentifierResolver(const ast::Function& scopeFunction, const DefMap& definitionMap);

        void operator()(ast::Integer& i, const std::monostate&) override;
        void operator()(ast::String& str, const std::monostate&) override;
        void operator()(ast::Operator& op, const std::monostate&) override;
        void operator()(ast::FunctionCall& func, const std::monostate&) override;
        void operator()(ast::ArgumentReference& arg, const std::monostate&) override;
    };
} // namespace ingot::semantics
