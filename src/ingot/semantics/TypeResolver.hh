#pragma once

#include <ingot/ast/Expression.hh>
#include <ingot/ast/Type.hh>

namespace ingot::semantics
{
    class TypeResolver : public ast::Expression::Visitor<ast::Type, size_t> {

    public:
        TypeResolver();

        // Input in tree traversal is currently size of undecided integer constants, but we probably want to deal with types in general
        size_t preop(ast::Operator& op, size_t input, OperatorSide side) const override;
        size_t preop(ast::FunctionCall& func, size_t input, size_t index) const override;
        ast::Type postop(ast::Integer& i, size_t size) const override;
        ast::Type postop(ast::String& str, size_t size) const override;
        ast::Type postop(ast::Operator& op, const std::pair<ast::Type, ast::Type>& results, size_t size) const override;
        ast::Type postop(ast::FunctionCall& func, const std::vector<ast::Type>& args, size_t size) const override;
        ast::Type postop(ast::ArgumentReference& arg, size_t size) const override;
    };
} // namespace ingot::semantics
