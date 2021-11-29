#pragma once

#include <ingot/ast/Expression.hh>

#include <map>
#include <string>

namespace ingot::semantics
{
    class IdentifierResolver : public ast::Expression::Visitor<> {
        using DefMap = std::map<std::string, const ast::FunctionDefinition&>;

        const ast::Function& m_scopeFunction;
        const DefMap& m_definitionMap;
    public:
        IdentifierResolver(const ast::Function& scopeFunction, const DefMap& definitionMap);

        std::monostate postop(ast::Integer& i, std::monostate) const override;
        std::monostate postop(ast::String& str, std::monostate) const override;
        std::monostate postop(ast::Operator& op, const std::pair<std::monostate, std::monostate>& argResults, std::monostate) const override;
        std::monostate postop(ast::FunctionCall& func, const std::vector<std::monostate>& argResults, std::monostate) const override;
        std::monostate postop(ast::ArgumentReference& arg, std::monostate) const override;
    };
} // namespace ingot::semantics
