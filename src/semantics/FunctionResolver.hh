#pragma once

#include <ast/Expression.hh>

#include <map>
#include <string>

namespace ingot::semantics
{
    class FunctionResolver : public ast::Expression::UpdateVisitor {
        using DefMap = std::map<std::string, const ast::FunctionDefinition&>;
        const DefMap& m_definitionMap;
    public:
        FunctionResolver(const DefMap& definitionMap);

        void operator()(ast::Integer& i) override;
        void operator()(ast::String& str) override;
        void operator()(ast::Operator& op) override;
        void operator()(ast::FunctionCall& func) override;
    };
} // namespace ingot::semantics
