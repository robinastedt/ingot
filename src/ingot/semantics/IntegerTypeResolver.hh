#pragma once

#include <ingot/ast/Expression.hh>

namespace ingot::semantics
{
    class IntegerTypeResolver : public ast::Expression::Visitor<std::monostate, size_t> {
        size_t preop(ast::Operator& op, size_t input, OperatorSide side) const override;
        size_t preop(ast::FunctionCall& func, size_t input, size_t index) const override;
        std::monostate postop(ast::Integer& i, size_t input) const override;
        std::monostate postop(ast::String& str, size_t input) const override;
        std::monostate postop(ast::Operator& op, const std::pair<std::monostate, std::monostate>& argResults, size_t input) const override;
        std::monostate postop(ast::FunctionCall& func, const std::vector<std::monostate>& argResults, size_t input) const override;
        std::monostate postop(ast::ArgumentReference& arg, size_t input) const override;
    };
} // namespace ingot::semantics
