#pragma once

#include <ingot/ast/Expression.hh>
#include <ingot/ast/Type.hh>

namespace ingot::semantics
{
    class TypeResolver : public ast::Expression::Visitor<ast::Type, ast::Type> {

    public:
        TypeResolver();

        ast::Type preop(ast::List& func, ast::Type input, size_t index) override;
        ast::Type preop(ast::Operator& op, ast::Type input, OperatorSide side) override;
        ast::Type preop(ast::FunctionCall& func, ast::Type input, size_t index) override;
        ast::Type preop(ast::Ternary& func, ast::Type input, TernaryPosition position) override;
        ast::Type postop(ast::Integer& i, ast::Type requiredType) override;
        ast::Type postop(ast::List& list, const std::vector<ast::Type>& elemResults, ast::Type requiredType) override;
        ast::Type postop(ast::Operator& op, const std::pair<ast::Type, ast::Type>& results, ast::Type requiredType) override;
        ast::Type postop(ast::FunctionCall& func, const std::vector<ast::Type>& args, ast::Type requiredType) override;
        ast::Type postop(ast::ArgumentReference& arg, ast::Type requiredType) override;
        ast::Type postop(ast::Ternary& ternary, const std::tuple<ast::Type, ast::Type, ast::Type>& results, ast::Type requiredType) override;
    };
} // namespace ingot::semantics
