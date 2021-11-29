#pragma once

#include <ingot/ast/Expression.hh>
#include <ingot/ast/Type.hh>

namespace ingot::semantics
{
    class TypeResolver : public ast::Expression::Visitor<ast::Type, ast::Type> {

    public:
        TypeResolver();

        ast::Type preop(ast::List& func, ast::Type input, size_t index) const override;
        ast::Type preop(ast::Operator& op, ast::Type input, OperatorSide side) const override;
        ast::Type preop(ast::FunctionCall& func, ast::Type input, size_t index) const override;
        ast::Type preop(ast::Ternary& func, ast::Type input, TernaryPosition position) const override;
        ast::Type postop(ast::Integer& i, ast::Type requiredType) const override;
        ast::Type postop(ast::List& list, const std::vector<ast::Type>& elemResults, ast::Type requiredType) const override;
        ast::Type postop(ast::Operator& op, const std::pair<ast::Type, ast::Type>& results, ast::Type requiredType) const override;
        ast::Type postop(ast::FunctionCall& func, const std::vector<ast::Type>& args, ast::Type requiredType) const override;
        ast::Type postop(ast::ArgumentReference& arg, ast::Type requiredType) const override;
        ast::Type postop(ast::Ternary& ternary, const std::tuple<ast::Type, ast::Type, ast::Type>& results, ast::Type requiredType) const override;
    };
} // namespace ingot::semantics
