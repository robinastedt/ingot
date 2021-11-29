#pragma once

#include <ingot/ast/Expression.hh>
#include <ingot/ast/Type.hh>

namespace ingot::semantics
{
    class TypeResolver : public ast::Expression::ConstVisitor<ast::Type> {

    public:
        TypeResolver();

        ast::Type postop(const ast::Integer& i, std::monostate) const override;
        ast::Type postop(const ast::String& str, std::monostate) const override;
        ast::Type postop(const ast::Operator& op, const std::pair<ast::Type, ast::Type>& results, std::monostate) const override;
        ast::Type postop(const ast::FunctionCall& func, const std::vector<ast::Type>& args, std::monostate) const override;
        ast::Type postop(const ast::ArgumentReference& arg, std::monostate) const override;
    };
} // namespace ingot::semantics
