#pragma once

#include <ast/Expression.hh>
#include <ast/Type.hh>

namespace ingot::semantics
{
    class TypeResolver : public ast::Expression::Visitor<ast::Type> {

    public:
        TypeResolver();

        ast::Type operator()(const ast::Integer& i) override;
        ast::Type operator()(const ast::String& str) override;
        ast::Type operator()(const ast::Operator& op, ast::Type result, ast::Type rhsResult) override;
        ast::Type operator()(const ast::FunctionCall& func) override;
    };
} // namespace ingot::semantics
